typedef struct Inner {
    int f0, f1, f2, f3, f4, f5;
} Inner;

typedef struct Outer {
    Inner *p0;
    Inner *p1;
} Outer;

extern Outer *D_00275B40;

void func_00150850(unsigned char *a0) {
    if (a0[5] == 0x63)
        return;

    D_00275B40->p0->f0 = 0;
    D_00275B40->p0->f1 = 0;
    D_00275B40->p0->f2 = 0;
    D_00275B40->p0->f3 = 0;
    D_00275B40->p0->f4 = 0;
    D_00275B40->p0->f5 = 0;
    D_00275B40->p1->f0 = 0;
    D_00275B40->p1->f1 = 0;
    D_00275B40->p1->f2 = 0;
    D_00275B40->p1->f3 = 0;
    D_00275B40->p1->f4 = 0;
    D_00275B40->p1->f5 = 0;
}
