// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Movie ring buffer: the readable region as up to two spans. r->base (+0x44)
// is the buffer, r->size (+0x48) its size and r->pos (+0x4C) the read
// position; write is the producer's position. The readable byte count is
// ((write + size - pos - 0x400) % size) rounded down to 1 KB. When it fits
// before the end of the buffer the first span gets it all and the second is
// empty; otherwise the first span runs to the end and the second starts at
// the buffer base.
typedef struct MovieRing {
    char pad[0x44];
    char *base;     /* 0x44 */
    int size;       /* 0x48 */
    int pos;        /* 0x4C */
} MovieRing;

void func_00206770(char **ptr1, int *len1, char **ptr2, int *len2, MovieRing *r, int write) {
    int size = r->size;
    int pos = r->pos;
    int avail = ((write + size - pos - 0x400) % size) >> 10 << 10;

    if (size - pos >= avail) {
        *ptr1 = r->base + pos;
        *len1 = avail;
        *ptr2 = 0;
        *len2 = 0;
        return;
    }
    *ptr1 = r->base + pos;
    *len1 = r->size - r->pos;
    *ptr2 = r->base;
    *len2 = avail - (r->size - r->pos);
}
