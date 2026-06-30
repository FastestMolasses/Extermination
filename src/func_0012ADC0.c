// NEARMISS func_0012ADC0  (vram 0x0012ADC0, 0x1FC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.02% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// List-scheduling + param-save-order artifact across a large global-store block. The control flow is now exact (flipping to `if (fparg0 > 0.0f) {...} else {neg path}` reproduced the target's c.le.s/bc1t forward-branch shape and matched the entire call-guard region). Residuals: (1) param save order ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001026A0(void *a, void *b, void *c);
extern void func_00102718(void *a, void *b, void *c);
extern void func_001027E0(void *a, void *b);
extern void func_001028D0(void *a, int b, int c);
extern void func_001029C0(void *a);
extern float func_001B1240(void *a, float b, float c);
extern int func_001B13F0(int a, float b);
extern char D_700036A0[];
extern char D_700036E0[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];

int func_0012ADC0(int arg0, int arg1, int arg2, float fparg0) {
    char *s = (char *)arg0 + 0x1F0;

    if (fparg0 > 0.0f) {
        if (func_001B13F0(arg2, fparg0) == 0) {
            return 0;
        }
    } else {
        if (func_001B13F0(arg2, -fparg0) != 0) {
            return 0;
        }
    }
    func_001029C0(D_700036A0);
    *(int *)0x700036C0 = *(int *)(s + 0x70);
    *(int *)0x700036C4 = *(int *)(s + 0x74);
    *(int *)0x700036C8 = *(int *)(s + 0x78);
    *(float *)0x700036B0 = *(float *)(s + 0x80);
    *(float *)0x700036B4 = *(float *)(s + 0x84);
    *(float *)0x700036B8 = *(float *)(s + 0x88);
    *(float *)0x700038B0 = *(float *)(s + 0x80);
    *(float *)0x700038B4 = *(float *)(s + 0x84);
    *(float *)0x700038B8 = *(float *)(s + 0x88);
    *(int *)0x700038BC = 0x3F800000;
    *(int *)0x700038C0 = *(int *)(s + 0x70);
    *(int *)0x700038C4 = *(int *)(s + 0x74);
    *(int *)0x700038C8 = *(int *)(s + 0x78);
    *(int *)0x700038CC = 0x3F800000;
    func_00102718(D_700038A0, D_700038B0, D_700038C0);
    *(float *)0x700036A0 = *(float *)0x700038A0;
    *(float *)0x700036A4 = *(float *)0x700038A4;
    *(float *)0x700036A8 = *(float *)0x700038A8;
    func_001027E0(D_700036E0, D_700036A0);
    func_001028D0(D_700038B0, arg1, arg2);
    func_001026A0(D_700038A0, D_700036E0, D_700038B0);
    *(int *)0x700038C0 = 0;
    *(int *)0x700038C4 = 0;
    *(int *)0x700038C8 = 0;
    *(float *)(s + 0xE8) = func_001B1240(D_700038C0, -*(float *)0x700038A0, -*(float *)0x700038A8);
    return 1;
}
