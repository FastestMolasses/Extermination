// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Builds two 4-word descriptor blocks on the stack and submits them to
// func_001D64A0(a0, &blk1, &blk2, &D_00253280, 0x40). blk1 = {0x7180, 0x7A90,
// 0xFFFFFF, 0}; blk2[0] is float_to_int(29056.0f + a1*2400.0f) (the float
// arg scaled and biased into an integer parameter), followed by {0x7AD0,
// 0xFFFFFF, 0}.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): pinning 2400.0f into a named
// local before the multiply makes mwcc emit `mul.s f12,f12,fconst` with the
// arg register first, matching CW's operand order (the 991202/default form
// canonicalizes the operands the other way). Verified objdiff 100% vs
// build/expected/func_001DBCB0.o.
extern int float_to_int(float);
extern void func_001D64A0(int a, void *b, void *c, void *d, int e);
extern char D_00253280[];

void func_001DBCB0(int a0, float a1) {
    int s1[4];
    int s2[4];
    float k = 2400.0f;

    a1 = a1 * k;
    s1[0] = 0x7180;
    s1[1] = 0x7A90;
    s1[2] = 0xFFFFFF;
    s1[3] = 0;
    s2[0] = float_to_int(29056.0f + a1);
    s2[1] = 0x7AD0;
    s2[2] = 0xFFFFFF;
    s2[3] = 0;
    func_001D64A0(a0, s1, s2, D_00253280, 0x40);
}
