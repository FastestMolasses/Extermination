/* Authored C99 acceleration for local delta matching and exact provenance scans.
 * No disc bytes, platform assembly, external libraries, or writable inputs. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct { uint64_t hash, position; } Slot;
typedef struct {
    const unsigned char *bytes;
    uint64_t length, capacity;
    Slot *slots;
    uint32_t *next;
} Index;

static uint64_t hash_bytes(const unsigned char *p, unsigned n) {
    uint64_t h = 0;
    while (n--) h = h * 257 + (uint64_t)*p++ + 1;
    return h;
}
static uint64_t power(unsigned n) {
    uint64_t x = 1;
    while (n--) x *= 257;
    return x;
}
static uint64_t bucket(uint64_t x) {
    x ^= x >> 33; x *= UINT64_C(0xff51afd7ed558ccd);
    x ^= x >> 33; x *= UINT64_C(0xc4ceb9fe1a85ec53);
    return x ^ (x >> 33);
}
static Index *allocate(const unsigned char *p, uint64_t n, uint64_t entries) {
    uint64_t capacity = 2;
    Index *h;
    if (entries > UINT64_MAX / 2) return NULL;
    while (capacity < entries * 2) {
        if (capacity > UINT64_MAX / 2) return NULL;
        capacity *= 2;
    }
    if (capacity > SIZE_MAX / sizeof(Slot)) return NULL;
    h = (Index *)calloc(1, sizeof(Index));
    if (!h) return NULL;
    h->slots = (Slot *)calloc((size_t)capacity, sizeof(Slot));
    if (!h->slots) { free(h); return NULL; }
    h->bytes = p; h->length = n; h->capacity = capacity;
    return h;
}
void em_index_free(Index *h) {
    if (h) { free(h->slots); free(h->next); free(h); }
}

/* Every 65-byte payload window is indexed; hash collisions are confirmed with
 * memcmp. Duplicate strings need one position only. */
Index *em_scan_make(const unsigned char *p, uint64_t n) {
    uint64_t i, hash, factor = power(64);
    Index *h = allocate(p, n, n >= 65 ? n - 64 : 0);
    if (!h || n < 65) return h;
    hash = hash_bytes(p, 65);
    for (i = 0; i + 65 <= n; ++i) {
        uint64_t at = bucket(hash) & (h->capacity - 1);
        while (h->slots[at].position) {
            if (h->slots[at].hash == hash && !memcmp(p + i, p + h->slots[at].position - 1, 65)) break;
            at = (at + 1) & (h->capacity - 1);
        }
        if (!h->slots[at].position) { h->slots[at].hash = hash; h->slots[at].position = i + 1; }
        if (i + 65 < n) hash = (hash - ((uint64_t)p[i] + 1) * factor) * 257 + p[i + 65] + 1;
    }
    return h;
}
int em_scan_chunk(Index *h, const unsigned char *p, uint64_t n, uint64_t starts, uint64_t *out) {
    uint64_t i, hash, factor = power(64);
    if (!h || n < 65 || !starts) return 0;
    if (starts > n - 64) starts = n - 64;
    hash = hash_bytes(p, 65);
    for (i = 0; i < starts; ++i) {
        uint64_t at = bucket(hash) & (h->capacity - 1);
        while (h->slots[at].position) {
            if (h->slots[at].hash == hash && !memcmp(p + i, h->bytes + h->slots[at].position - 1, 65)) {
                out[0] = i; out[1] = h->slots[at].position - 1; return 1;
            }
            at = (at + 1) & (h->capacity - 1);
        }
        if (i + 1 < starts) hash = (hash - ((uint64_t)p[i] + 1) * factor) * 257 + p[i + 65] + 1;
    }
    return 0;
}

/* Sparse 32-byte base anchors find every shared run of at least 65 bytes.
 * Entries with equal hash retain every source position, including collisions. */
Index *em_match_make(const unsigned char *p, uint64_t n) {
    uint64_t blocks = n / 32, i;
    Index *h;
    if (blocks >= UINT32_MAX) return NULL;
    h = allocate(p, n, blocks);
    if (!h) return NULL;
    if (blocks) {
        h->next = (uint32_t *)calloc((size_t)blocks, sizeof(uint32_t));
        if (!h->next) { em_index_free(h); return NULL; }
    }
    /* Reverse insertion makes lower source offsets win deterministically. */
    for (i = blocks; i-- > 0;) {
        uint64_t hash = hash_bytes(p + i * 32, 32), at = bucket(hash) & (h->capacity - 1);
        while (h->slots[at].position && h->slots[at].hash != hash) at = (at + 1) & (h->capacity - 1);
        h->next[i] = (uint32_t)h->slots[at].position;
        h->slots[at].hash = hash; h->slots[at].position = i + 1;
    }
    return h;
}
int em_match_next(Index *h, const unsigned char *edited, uint64_t n, uint64_t start, uint64_t *out) {
    uint64_t i, hash, factor = power(31);
    if (!h || start > n || n - start < 32) return 0;
    hash = hash_bytes(edited + start, 32);
    for (i = start; i + 32 <= n; ++i) {
        uint64_t at = bucket(hash) & (h->capacity - 1), node;
        while (h->slots[at].position && h->slots[at].hash != hash) at = (at + 1) & (h->capacity - 1);
        node = h->slots[at].position;
        while (node) {
            uint64_t p = (node - 1) * 32, left = 0, right = 32;
            if (!memcmp(h->bytes + p, edited + i, 32)) {
                while (left < 31 && left < p && left < i - start && h->bytes[p - left - 1] == edited[i - left - 1]) ++left;
                while (p + right < h->length && i + right < n && h->bytes[p + right] == edited[i + right]) ++right;
                if (left + right >= 64) { out[0] = i - left; out[1] = p - left; out[2] = left + right; return 1; }
            }
            node = h->next[node - 1];
        }
        if (i + 32 < n) hash = (hash - ((uint64_t)edited[i] + 1) * factor) * 257 + edited[i + 32] + 1;
    }
    return 0;
}
void em_xor(const unsigned char *a, const unsigned char *b, unsigned char *out, uint64_t n) {
    uint64_t i;
    for (i = 0; i < n; ++i) out[i] = a[i] ^ b[i];
}
