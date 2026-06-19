// COMPILER: eegcc
// CFLAGS: -O2
// If a0->ch is armed (f4 && f8), flush it: snapshot timer, record elapsed,
// disarm; return whether a flush occurred.
extern int D_00241404;
extern void sub_the_second_field_is_missing(int a0);

struct ch {
    int f0;
    int f4;
    int f8;
    char pad[0xAC - 0xC];
    int fAC;
};
struct obj {
    char pad8[0x8];
    int f8;
    char pad40[0x40 - 0xC];
    struct ch *f40;
};

int func_0010A140(struct obj *a0) {
    struct ch *c = a0->f40;
    int r = 0;
    if (c->f4 != 0 && c->f8 != 0) {
        sub_the_second_field_is_missing(D_00241404);
        a0->f8 = D_00241404 - c->fAC;
        r = 1;
        c->f4 = 0;
    }
    return r;
}
