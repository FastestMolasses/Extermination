// NEARMISS func_00181F60  (vram 0x00181F60, 0x128 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.46% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two compiler artifacts: (1) instruction scheduling -- mwcc 2.3.3 emits jal func_0019A570 before the swc1 store of the +25.0 angle update, target emits the store first then the call; (2) FP register coloring -- uniform ~2 FP-register shift (const->f1 target vs f3 ours; func_0011E620 2nd arg f13 vs...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 94.46% (mwcc 2.3.3). Logic fully recovered. Resets the global vec/matrix
// scratch at 0x700038A0 (=0,0,4.0,1.0), builds a transform via func_001026A0(
// &D_700038B0, self+0xD0, &D_700038A0) then func_001031E0(&D_700038C0, &D_700038B0),
// advances the angle accumulator *0x700038C4 += 25.0, and if func_0019A570(&D_700038B0,
// &D_700038C0, 4, 0) succeeds AND the global state object (*0x700031D0) is in mode
// 0x36 (byte at +0x1A), runs func_00199DB0(self+0x290), computes a heading
// func_0011E620(-(*+0x3C), *+0x34) into *0x70003A20, rotates it by func_001B1470(
// pi/2 + *0x70003A20), writes the result to *0x70003A20 and self+0x218, and returns 1;
// all other paths return 0. Two residuals: (1) mwcc 2.3.3 schedules the swc1 store of
// the 0x38C4 angle update relative to the func_0019A570 call differently than the
// target (call placed before vs after the store); (2) the FP block is uniformly
// shifted ~2 FP registers (const->f1 target vs f3 ours; func_0011E620 2nd arg f13 vs
// f15). Both are mwcc scheduling/FP-coloring artifacts (permuter), not the clean-store
// nop. All opcodes/relocations/constants otherwise correct.
extern void func_001026A0(void *dst, void *src, void *p);
extern void func_001031E0(void *dst, void *src);
extern int func_0019A570(void *a, void *b, int n, int m);
extern void func_00199DB0(void *p);
extern float func_0011E620(float dx, float dy);
extern float func_001B1470(float a);
extern float D_700038B0;
extern float D_700038C0;
extern float D_700038A0;

int func_00181F60(char *self) {
    char *p;

    *(float *)0x700038A0 = 0.0f;
    *(float *)0x700038A4 = 0.0f;
    *(float *)0x700038A8 = 4.0f;
    *(float *)0x700038AC = 1.0f;
    func_001026A0(&D_700038B0, self + 0xD0, &D_700038A0);
    func_001031E0(&D_700038C0, &D_700038B0);
    *(float *)0x700038C4 = *(float *)0x700038C4 + 25.0f;
    if (func_0019A570(&D_700038B0, &D_700038C0, 4, 0) != 0) {
        p = *(char **)0x700031D0;
        if (*(unsigned char *)(p + 0x1A) == 0x36) {
            func_00199DB0(self + 0x290);
            p = *(char **)0x700031D0;
            *(float *)0x70003A20 = func_0011E620(-*(float *)(p + 0x3C), *(float *)(p + 0x34));
            *(float *)0x70003A20 = func_001B1470(1.5707964f + *(float *)0x70003A20);
            *(float *)(self + 0x218) = *(float *)0x70003A20;
            return 1;
        }
    }
    return 0;
}
