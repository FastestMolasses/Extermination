// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Animation-clip update for an entity (a0): resolves the active clip id via
// func_0017B490(a0, 1, *(u8*)(a0+0x235), 1) (sign-extended to 16 bits),
// converts the per-frame sample value func_001C61D0(*(int*)(a0+0x40), clip)
// to float and stashes it in the global scratch *(float*)0x70003A20, then
// hands the clip id plus an 8.0f blend weight and the delta (stored value
// minus the clip's base time D_00248740[*(u8*)(a0+0x235)]) to
// anim_clip_arbiter for the actual clip-change decision.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): under the pinned 991202 build
// the lone residual was the clip-id arg paddub being scheduled differently
// (walled at 94.6%); 2.3.3 is byte-identical. Verified objdiff 100% vs
// build/expected/func_0017B5C0.o.
extern short func_0017B490(int, int, int, int);
extern int func_001C61D0(int, int);
extern void anim_clip_arbiter(int, int, float, float);
extern float D_00248740[];

void func_0017B5C0(char *a0) {
    int clip;
    float v;

    clip = (short)func_0017B490((int)a0, 1, *(unsigned char *)(a0 + 0x235), 1);
    v = (float)func_001C61D0(*(int *)(a0 + 0x40), clip);
    *(float *)0x70003A20 = v;
    anim_clip_arbiter((int)a0, clip, 8.0f,
                      *(float *)0x70003A20 - D_00248740[*(unsigned char *)(a0 + 0x235)]);
}
