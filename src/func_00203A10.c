// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Audio stream ring: consumes up to n bytes. The 0x50000-byte buffer is
// followed by the read position, the filled count and the size; n is
// clamped to the free space (size - count), the position advances modulo
// the size and the count grows by n. Returns the clamped n.
typedef struct StreamRing {
    char data[0x50000];
    int pos;    /* 0x50000 */
    int count;  /* 0x50004 */
    int size;   /* 0x50008 */
} StreamRing;

int func_00203A10(StreamRing *r, int n) {
    int size = r->size;
    int avail = size - r->count;

    n = (n < avail) ? n : avail;
    r->pos = (r->pos + n) % size;
    r->count += n;
    return n;
}
