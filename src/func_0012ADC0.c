// NEARMISS func_0012ADC0  (vram 0x0012ADC0, 0x1FC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.87% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0).
// Matching wave 2026-10-27 (was 84.17%): the copies out of the entity's
// position/rotation vectors (+0x70.., +0x80..) are FLOAT copies (the original
// moves them through FPU registers), and the copy of the 0x700038A0 result
// vector into 0x700036A0 reads all three components into locals before
// storing them.
// Remaining differences: FP register colouring/scheduling of that three-float
// copy only (the original loads x, y, z in order into f2, f1, f0; this C loads
// x and z in swapped slots). Load/decl-order sweeps, struct copy, pointer and
// relocated-extern spellings and a 540 s 2.3.3 permuter run did not close it.
//
// CORRECTED 2026-09-27 (AREA01 wave-2 review, checked against the original
// instructions; original-instruction harness: 0 differing cases).
// func_001B13F0 takes (arg2, arg1, +/-fparg0): the original passes arg1 as
// its second argument (001B13F0 hands both pointers to func_001028D0).
// The old two-argument call compiled to the same code only
// because $a1 still held arg1 at the call.
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
extern int func_001B13F0(int a, int b, float f);
extern char D_700036A0[];
extern char D_700036E0[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];

int func_0012ADC0(int arg0, int arg1, int arg2, float fparg0) {
    char *s = (char *)arg0 + 0x1F0;

    if (fparg0 > 0.0f) {
        if (func_001B13F0(arg2, arg1, fparg0) == 0) {
            return 0;
        }
    } else {
        if (func_001B13F0(arg2, arg1, -fparg0) != 0) {
            return 0;
        }
    }
    func_001029C0(D_700036A0);
    *(float *)0x700036C0 = *(float *)(s + 0x70);
    *(float *)0x700036C4 = *(float *)(s + 0x74);
    *(float *)0x700036C8 = *(float *)(s + 0x78);
    *(float *)0x700036B0 = *(float *)(s + 0x80);
    *(float *)0x700036B4 = *(float *)(s + 0x84);
    *(float *)0x700036B8 = *(float *)(s + 0x88);
    *(float *)0x700038B0 = *(float *)(s + 0x80);
    *(float *)0x700038B4 = *(float *)(s + 0x84);
    *(float *)0x700038B8 = *(float *)(s + 0x88);
    *(int *)0x700038BC = 0x3F800000;
    *(float *)0x700038C0 = *(float *)(s + 0x70);
    *(float *)0x700038C4 = *(float *)(s + 0x74);
    *(float *)0x700038C8 = *(float *)(s + 0x78);
    *(int *)0x700038CC = 0x3F800000;
    func_00102718(D_700038A0, D_700038B0, D_700038C0);
    {
        float x, y, z;
        x = *(float *)0x700038A0;
        z = *(float *)0x700038A8;
        y = *(float *)0x700038A4;
        *(float *)0x700036A0 = x;
        *(float *)0x700036A4 = y;
        *(float *)0x700036A8 = z;
    }
    func_001027E0(D_700036E0, D_700036A0);
    func_001028D0(D_700038B0, arg1, arg2);
    func_001026A0(D_700038A0, D_700036E0, D_700038B0);
    *(int *)0x700038C0 = 0;
    *(int *)0x700038C4 = 0;
    *(int *)0x700038C8 = 0;
    *(float *)(s + 0xE8) = func_001B1240(D_700038C0, -*(float *)0x700038A0, -*(float *)0x700038A8);
    return 1;
}
