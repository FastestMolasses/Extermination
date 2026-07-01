// NEARMISS func_00210F30  (vram 0x00210F30, 0x30C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.89% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation/instruction-scheduling permutation, two spots: (1) the D_700038A0/B0/C0 post-transform scale block (16.0f*(1792+x) / 16.0f*(1936+x/2)) recomputes the same 6-store sequence but mwcc233 colors the FP temps into different ft0/ft1/ft2/ft3 odd/half registers and reorders the load/c...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Gameplay-state setup driver, gated by the global mode byte D_00810700: returns
// immediately for modes 0x12 and 0x15, otherwise builds a camera/transform rig.
// Caches arg0's +0x40/+0x48 floats (x/z), seeds an identity-ish matrix at
// D_700036A0 and a scaled matrix at D_700036E0 (0.8/1.0/0.5 diagonal-ish consts),
// looks up a per-area base record via D_00265890[D_00810142] (0xC-byte stride,
// indexed again by D_00810144), reads its +8 float and combines with the global
// heading D_00810374 minus pi/2, wraps the angle through func_001B1470, composes
// D_700036A0 via func_001026D0/func_00102BB0, then builds a translate-only matrix
// at D_700038A0 from (256+z, 164-x), transforms it through func_001031E0, copies
// three lookup vectors D_0026A990/9A0/9B0 into D_700038A0/B0/C0 via func_00102948,
// rotates each through func_001026A0(., D_700036A0, .), rescales all three into
// screen/tile-space via the 16*(1792+v) / 16*(1936+v/2) per-axis formulas, then
// submits the packet via func_00207D00(1,3) and func_00208040(1, ..., 0x802040A0).
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001026D0(void *a, void *b, void *c);
extern void func_00102948(void *a, void *b);
extern void func_001029C0(void *a);
extern void func_00102BB0(void *a, void *b, float f);
extern void func_001031E0(void *a, void *b);
extern float func_001B1470(float a);
extern void func_00207D00(int a, int b);
extern void func_00208040(int a, void *b, void *c, void *d, unsigned long long e);
extern int D_00265890[];
extern char D_0026A990[];
extern char D_0026A9A0[];
extern char D_0026A9B0[];
extern unsigned char D_00810142;
extern unsigned char D_00810144;
extern float D_00810374;
extern unsigned char D_00810700;
extern char D_700036A0[];
extern char D_700036D0[];
extern char D_700036E0[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];

void func_00210F30(char *arg0) {
    int base;
    float fx, fz;

    if (D_00810700 != 0x12) {
        if (D_00810700 == 0x15) {
            return;
        }
        goto body;
    }
    return;
body:
    fx = *(float *)(arg0 + 0x40);
    fz = *(float *)(arg0 + 0x48);
    func_001029C0(D_700036A0);
    func_001029C0(D_700036E0);
    *(float *)0x700036E0 = 0.8f;
    *(float *)0x700036F4 = 1.0f;
    *(float *)0x70003708 = 0.5f;
    *(int *)0x7000371C = 0;
    base = D_00265890[D_00810142];
    *(float *)0x70003A20 = (D_00810374 - *(float *)(base + D_00810144 * 0xC + 8)) - 1.5707964f;
    *(float *)0x70003A20 = func_001B1470(*(float *)0x70003A20);
    func_001026D0(D_700036A0, D_700036E0, D_700036A0);
    func_00102BB0(D_700036A0, D_700036A0, *(float *)0x70003A20);
    *(float *)0x700038A0 = 256.0f + fz;
    *(int *)0x700038A4 = 0;
    *(float *)0x700038A8 = 164.0f - fx;
    *(float *)0x700038AC = 1.0f;
    func_001031E0(D_700036D0, D_700038A0);
    func_00102948(D_700038A0, D_0026A990);
    func_00102948(D_700038B0, D_0026A9A0);
    func_00102948(D_700038C0, D_0026A9B0);
    func_001026A0(D_700038A0, D_700036A0, D_700038A0);
    func_001026A0(D_700038B0, D_700036A0, D_700038B0);
    func_001026A0(D_700038C0, D_700036A0, D_700038C0);
    {
        float b0v = 1792.0f + *(float *)0x700038B0;
        float c0v = *(float *)0x700038C0;
        *(float *)0x700038A0 = 16.0f * (1792.0f + *(float *)0x700038A0);
        *(float *)0x700038A8 = 16.0f * (1936.0f + *(float *)0x700038A8 / 2.0f);
        *(float *)0x700038B0 = 16.0f * b0v;
        *(float *)0x700038C0 = 16.0f * (1792.0f + c0v);
        *(float *)0x700038B8 = 16.0f * (1936.0f + *(float *)0x700038B8 / 2.0f);
        *(float *)0x700038C8 = 16.0f * (1936.0f + *(float *)0x700038C8 / 2.0f);
    }
    func_00207D00(1, 3);
    func_00208040(1, D_700038A0, D_700038B0, D_700038C0, 0x802040A0ULL);
}
