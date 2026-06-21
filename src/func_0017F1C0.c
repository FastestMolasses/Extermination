// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Effect/shadow setup variant: writes four params into the global block at
// 0x700038A0 (int 0, then floats 10.0/5.0/1.0 at A4/A8/AC), then runs the
// shared pair func_001026A0(&D_700038B0, self+0xD0, &D_700038A0) and
// func_0019AD00(self, &D_700038B0, 7). Sibling of func_001818D0 (different
// constants; this one is void).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the lone
// residual under 991202 was wall #13 (the scheduler hoists `paddub a0,s0,zero`
// out of the second jal's delay slot and fills it with a safe op). 2.3.3 leaves
// the paddub in the delay slot, so this readable C is byte-identical (objdiff
// 100.0). 991202 caps at 86.67%.
extern void func_001026A0(int, int, int);
extern void func_0019AD00(int, int, int);
extern int D_700038A0;
extern int D_700038B0;

void func_0017F1C0(int a0) {
    *(int *)0x700038A0 = 0;
    *(float *)0x700038A4 = 10.0f;
    *(float *)0x700038A8 = 5.0f;
    *(float *)0x700038AC = 1.0f;
    func_001026A0((int)&D_700038B0, a0 + 0xD0, (int)&D_700038A0);
    func_0019AD00(a0, (int)&D_700038B0, 7);
}
