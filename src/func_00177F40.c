// NEARMISS func_00177F40  (vram 0x00177F40, 0x140 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.75% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring permutation. Body and structure fully recovered. The target computes sp[0]=*0x70003050-1.5f*(*0x70003060) and sp[2]=*0x70003058-1.5f*(*0x70003068) using a specific physical FP-register assignment (f4/f3/f2/f1/f0, with 1.5f materialized into f3 early and loads of 0x3060/0x3058...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001029C0(void *);
extern void func_00102BB0(void *, void *, float);
extern void func_00102918(void *, void *, float *);
extern void func_001026A0(void *, void *, void *);
extern int func_0019A570(void *, void *, int, int);
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700038D0[];

int func_00177F40(float fparg0) {
    float sp[3];

    sp[0] = *(float *)0x70003050 - 1.5f * *(float *)0x70003060;
    sp[1] = fparg0;
    sp[2] = *(float *)0x70003058 - 1.5f * *(float *)0x70003068;
    func_001029C0(D_700036A0);
    func_00102BB0(D_700036A0, D_700036A0, *(float *)0x700031E4);
    func_00102918(D_700036A0, D_700036A0, sp);
    *(int *)0x700038A0 = 0;
    *(int *)0x700038B0 = 0;
    *(int *)0x700038A4 = 0x3F800000;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    *(int *)0x700038B4 = 0xBF800000;
    *(int *)0x700038B8 = 0;
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(D_700038C0, D_700036A0, D_700038A0);
    func_001026A0(D_700038D0, D_700036A0, D_700038B0);
    return func_0019A570(D_700038C0, D_700038D0, 6, 0) != 0 ? 1 : 0;
}
