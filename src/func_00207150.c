// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Copies a two-span source (s1 / sn1 bytes, then s2 / sn2) into a two-span
// destination (d1 / n1, then d2 / n2), as ring buffers wrap. Returns 0 when
// the destination holds fewer than sn1 + sn2 bytes, else that total.
// block_copy(dst, src, n) copies bytes.
extern void block_copy(char *dst, char *src, int n);

int func_00207150(char *d1, int n1, char *d2, int n2, char *s1, int sn1, char *s2, int sn2) {
    int rest;
    int total = sn1 + sn2;

    if (n1 + n2 < total) {
        return 0;
    }
    if (!(sn1 < n1)) {
        block_copy(d1, s1, n1);
        block_copy(d2, s1 + n1, sn1 - n1);
        block_copy(d2 + sn1 - n1, s2, sn2);
    } else {
        rest = n1 - sn1;
        if (!(sn2 < rest)) {
            block_copy(d1, s1, sn1);
            block_copy(d1 + sn1, s2, rest);
            block_copy(d2, s2 + n1 - sn1, sn2 - rest);
        } else {
            block_copy(d1, s1, sn1);
            block_copy(d1 + sn1, s2, sn2);
        }
    }
    return total;
}
