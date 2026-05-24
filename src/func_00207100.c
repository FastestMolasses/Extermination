// CFLAGS: -O4,p -sdatathreshold 0
typedef struct { long long x, y; } Pair64;
extern Pair64 D_002DF788;
extern void func_00204E90(Pair64 *, Pair64 *);

int func_00207100(void *a0, void *a1) {
    Pair64 buf;
    Pair64 buf2;
    void *s0 = a1;
    func_00204E90(&D_002DF788, &buf);
    (void)buf2;
    *(long long *)((char *)s0 + 0x8) = buf.x;
    *(long long *)((char *)s0 + 0x10) = buf.y;
    return 1;
}
