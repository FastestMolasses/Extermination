// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Records a bone/segment count (arg1) at p+0xC; if it exceeds the gp-rel limit
// D_00275BCC it flags state 3 at p+4 and returns 0. Otherwise it fills count
// entries at p+0x110.. (stride 4) each with func_001AF780(), runs
// anim_bone_array_setup(count), stores the count at p+9, flags state 1 at p+4,
// and returns 1.
extern void anim_bone_array_setup(unsigned char a);
extern int func_001AF780(void);
extern short D_00275BCC;

int func_001F87E0(int a0, unsigned char a1) {
    unsigned char *p = (unsigned char *)a0;
    int i;
    unsigned char n;
    unsigned char *q;

    *(unsigned char *)(p + 0xC) = a1;
    if (D_00275BCC < (int)*(unsigned char *)(p + 0xC)) {
        *(unsigned char *)(p + 4) = 3;
        return 0;
    }
    i = 0;
    q = p;
    goto test;
loop:
    *(int *)(q + 0x110) = func_001AF780();
    q += 4;
    i += 1;
test:
    n = *(unsigned char *)(p + 0xC);
    if (i < (int)n) {
        goto loop;
    }
    anim_bone_array_setup(n);
    *(unsigned char *)(p + 9) = *(unsigned char *)(p + 0xC);
    *(unsigned char *)(p + 4) = 1;
    return 1;
}
