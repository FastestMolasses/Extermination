// NEARMISS func_0022E630  (vram 0x0022E630, 0x1C0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.01% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP odd-half register coloring + scheduling permutation. Body/structure/signature/all-globals/packed-GS-call fully recovered; integer tail byte-identical. Target colors dx/dy and the two scale-chain temporaries into FP odd-half regs (fs0f/fv0f/fv1f) and schedules the lui/mtc1 constant loads differ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Proximity-gated radar/minimap blip draw. arg0 is the viewer/player entity
// (world pos x at +0xA0, z at +0xA8, facing angle at +0xC4); arg1 is a target
// entity (world pos x at +0xB0, z at +0xB8). Computes the squared planar
// distance between them and only proceeds if within 200^2 (40000). When in
// range: builds a 2D rotation matrix at D_700036A0 from func_001B1470 of the
// negated facing (2*PI - (PI + angle)), stashes the relative offset (dx, dy)
// plus a 1.0 scale into the uncached scratch vector at 0x700038A0, rotates it
// into 0x700038B0 via func_001026A0, then maps the rotated x/z to integer
// screen coords (16.0 * (1792 + 96 + 0.39375*rx)) and
// (16.0 * (1936 + (340 + 0.45*rz)/2)) and issues the blip primitive through
// func_00207E40 with color 0x80008080 and packed GS reg 0x20041605113222CD.
extern void func_001029C0(void *a);
extern void func_00102BB0(void *a, void *b, float f);
extern void func_001026A0(void *a, void *b, void *c);
extern float func_001B1470(float f);
extern int float_to_int(float f);
extern void func_00207E40(int a, int b, int c, int d, int e, int f, unsigned long long g);
extern int D_700036A0;
extern int D_700038A0;
extern int D_700038B0;

void func_0022E630(char *arg0, char *arg1) {
    float dx;
    float dy;
    int x;

    dx = *(float *)(arg1 + 0xB0) - *(float *)(arg0 + 0xA0);
    dy = *(float *)(arg1 + 0xB8) - *(float *)(arg0 + 0xA8);
    if (dx * dx + dy * dy <= 40000.0f) {
        func_001029C0(&D_700036A0);
        func_00102BB0(&D_700036A0, &D_700036A0, func_001B1470(6.2831855f - (3.1415927f + *(float *)(arg0 + 0xC4))));
        *(float *)0x700038A0 = dx;
        *(int *)0x700038A4 = 0;
        *(float *)0x700038A8 = dy;
        *(int *)0x700038AC = 0x3F800000;
        func_001026A0(&D_700038B0, &D_700036A0, &D_700038A0);
        x = float_to_int(16.0f * (1792.0f + (96.0f + (0.39374998f * *(float *)0x700038B0))));
        func_00207E40(1, x, float_to_int(16.0f * (1936.0f + ((340.0f + (0.45f * *(float *)0x700038B8)) / 2.0f))), 8, 8, 0x80008080, 0x20041605113222CDULL);
    }
}
