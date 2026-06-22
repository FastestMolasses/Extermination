// COMPILER: mwcc233
// CFLAGS: -O4,p
//
// Stores a count (arg1) into the object's byte +0xC. If it exceeds the global
// limit (gp-rel s16 D_00275BCC) it bails: sets state byte +4 = 3 and returns
// 0. Otherwise loops count times, calling func_001AF780(count) and storing
// each result into the +0x110 slot array (stride 4), then runs
// anim_bone_array_setup(count), copies count into byte +9, and returns 1.
// The `i = 0; p = arg0;` ordering (counter initialized before the pointer)
// is load-bearing: CW emits the s1 counter-init ahead of the s2 pointer-copy
// in the loop preheader's b-delay-slot.
//
// Built with mwcc 2.3.3: the 991202 residual is loop-body register-allocation
// / delay-slot-nop scheduling that CW + 2.3.3 share. Verified objdiff 100%
// vs build/expected/func_001F1550.o.
extern int func_001AF780(unsigned char);
extern void anim_bone_array_setup(unsigned char);
extern short D_00275BCC;

int func_001F1550(char *arg0, unsigned char arg1) {
    int i;
    char *p;

    *(unsigned char *)(arg0 + 0xC) = arg1;
    if (D_00275BCC < (int)*(unsigned char *)(arg0 + 0xC)) {
        *(char *)(arg0 + 4) = 3;
        return 0;
    }
    i = 0;
    p = arg0;
    for (; i < (int)*(unsigned char *)(arg0 + 0xC); i++) {
        *(int *)(p + 0x110) = func_001AF780(*(unsigned char *)(arg0 + 0xC));
        p += 4;
    }
    anim_bone_array_setup(*(unsigned char *)(arg0 + 0xC));
    *(unsigned char *)(arg0 + 9) = *(unsigned char *)(arg0 + 0xC);
    return 1;
}
