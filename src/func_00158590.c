// NEARMISS func_00158590  (vram 0x00158590, 0x278 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.95% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-half coloring in the three vec3-add chains plus argument-emission-order for func_001028D0's call. Not the clean-store delay-slot nop; 2.3.3 does not close it. Permuter/regalloc-scheduling territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001028D0(void *dst, void *a, void *b);
extern float func_00102738(void *a, void *b);
extern void func_00102948(void *dst, void *src);
extern float func_0011E2A8(float x);
extern float func_0011DE90(float x);
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_001F4CC0(void *a, void *b);
extern char D_008105D0[];
extern char D_700038A0[];
extern char D_700038B0[];

void func_00158590(char *p, int arg1, int mode) {
    int sp40[4];

    if (mode != -2) {
        func_001028D0(sp40, D_008105D0, p + 0xB0);
        if (func_00102738(sp40, p + 0xF0) < 0.0f) {
            return;
        }
    }

    if (mode != -2) {
        if (mode == -1) {
            *(float *)0x700038A0 = *(float *)(p + 0xB0) + *(float *)0x700038B0;
            *(float *)0x700038A4 = *(float *)(p + 0xB4) + *(float *)0x700038B4;
            *(float *)0x700038A8 = *(float *)(p + 0xB8) + *(float *)0x700038B8;
            *(int *)0x700038AC = 0x3F800000;
        } else if (mode == 1) {
            func_00102948(D_700038A0, p + 0xB0);
            *(int *)0x700038AC = 0x3F800000;
        } else if (mode != 0) {
            *(float *)0x700038B0 = 1.5f * func_0011E2A8(*(float *)(p + 0xC4));
            *(float *)0x700038B8 = 1.5f * func_0011DE90(*(float *)(p + 0xC4));
            *(int *)0x700038B4 = 0;
            *(float *)0x700038A0 = *(float *)(p + 0xB0) + *(float *)0x700038B0;
            *(float *)0x700038A4 = *(float *)(p + 0xB4) + *(float *)0x700038B4;
            *(float *)0x700038A8 = *(float *)(p + 0xB8) + *(float *)0x700038B8;
            *(int *)0x700038AC = 0x3F800000;
        } else {
            *(float *)0x700038A0 = 0.4f;
            *(float *)0x700038A4 = -0.1f;
            *(float *)0x700038A8 = 0.25f;
            *(int *)0x700038AC = 0x3F800000;
            func_001026A0(D_700038A0, p + 0xD0, D_700038A0);
        }
    }

    if (arg1 == 0) {
        *(int *)0x700038B0 = 0x80;
        *(int *)0x700038B4 = 0;
        *(int *)0x700038B8 = 0;
    } else {
        *(int *)0x700038B0 = 0;
        *(int *)0x700038B4 = 0x80;
        *(int *)0x700038B8 = 0;
    }
    *(int *)0x700038BC = 0x80;
    func_001F4CC0(D_700038A0, D_700038B0);
}
