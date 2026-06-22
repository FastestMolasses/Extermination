// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// arg0 is unused (occupies the first arg register). Calls func_001EF9D0
// with the float at arg2->0xC to obtain object p; if non-null, initializes
// two sub-structures (p+0xB0 from arg1, p+0xC0 from arg2) via func_00102948
// and writes 1.0f (raw 0x3F800000) to p->0xBC. Returns p.
extern int func_001EF9D0(float);
extern void func_00102948(int, int);

int func_001EFD90(int arg0, int arg1, int arg2) {
    int p;
    p = func_001EF9D0(*(float *)(arg2 + 0xC));
    if (p != 0) {
        func_00102948(p + 0xB0, arg1);
        func_00102948(p + 0xC0, arg2);
        *(int *)(p + 0xBC) = 0x3F800000;
    }
    return p;
}
