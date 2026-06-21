// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Effect/shadow setup variant returning a success flag. Writes the global block
// at 0x700038A0 (int 0, int 0, float 20.0, float 1.0 at A0/A4/A8/AC), runs
// func_001026A0(&D_700038B0, self+0xD0, &D_700038A0), then returns the boolean
// normalization of func_0019AD00(self, &D_700038B0, 7) (1 if nonzero else 0 —
// the addiu v1,1 / movz v1,zero,v0 / paddub idiom). Sibling of func_0017F1C0.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: under
// 991202 the scheduler hoists `paddub a0,s0,zero` out of the jal delay slot
// (wall #13), capping at 87.5%. 2.3.3 leaves it in the delay slot → objdiff
// 100.0.
extern int func_001026A0(int, int, int);
extern int func_0019AD00(int, int, int);
extern int D_700038A0;
extern int D_700038B0;

int func_001818D0(int a0) {
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0;
    *(float *)0x700038A8 = 20.0f;
    *(float *)0x700038AC = 1.0f;
    func_001026A0((int)&D_700038B0, a0 + 0xD0, (int)&D_700038A0);
    return func_0019AD00(a0, (int)&D_700038B0, 7) ? 1 : 0;
}
