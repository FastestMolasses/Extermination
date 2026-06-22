// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Initializes an actor's bone/segment array. Calls func_001CA5E0 (passing the
// incoming a0/a1 through, count 5), stores a count byte at p+0xC from
// func_001C6150(p->[0x44]). If that count+1 exceeds the gp-rel limit
// D_00275BCC, it flags state 3 at p+4 and returns 1. Otherwise it fills count
// entries at p+0x110.. (stride 4) each with func_001AF780(), records the count
// at p+9, runs anim_bone_array_setup(count), stores a final func_001AF780()
// handle at p+0x90, and returns 0.
//
// mwcc 2.3.3 matches byte-identical; 991202 differs on the call scheduling and
// the test-at-bottom loop lowering (residual ~87%). The bottom-test goto loop
// plus the dead i=0 in the early-return path reproduce the compiler's
// duplicated counter-clear; D_00275BCC is gp-rel (-sdatathreshold 4).
extern void anim_bone_array_setup(unsigned char a);
extern int func_001AF780(void);
extern unsigned char func_001C6150(int a);
extern void func_001CA5E0(int a, int b, int c);
extern short D_00275BCC;

int func_001D0C80(int a0, int a1) {
    unsigned char *p = (unsigned char *)a0;
    int i;
    unsigned char n;
    unsigned char *q;

    func_001CA5E0(a0, a1, 5);
    *(unsigned char *)(p + 0xC) = func_001C6150(*(int *)(p + 0x44));
    if (D_00275BCC < (*(unsigned char *)(p + 0xC) + 1)) {
        *(unsigned char *)(p + 4) = 3;
        i = 0;
        return 1;
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
    *(unsigned char *)(p + 9) = n;
    anim_bone_array_setup(*(unsigned char *)(p + 0xC));
    *(int *)(p + 0x90) = func_001AF780();
    return 0;
}
