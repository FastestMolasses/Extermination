// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038A0 0x700038B0 0x700038C0

//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 84.89% NEARMISS).
// Fixes read from the original instructions (spelling taken from the byte-matched
// twin func_0022E7F0, which ends in the same transform/scale/submit block):
// - D_700038A0/B0/C0 are relocated scratchpad float vectors (SPAD externs) indexed
//   as arrays, so the rescale block's loads may move ahead of the earlier stores.
// - The packet argument of func_00208040 is the sign-extended int 0x802040A0, not a
//   zero-extended 64-bit constant.
// - D_00265890 is a table of float record pointers; the sub-record is indexed as
//   base[sub * 3 + 2]; the mode guard is one OR test.
//
// Gameplay-state setup driver, gated by the global mode byte D_00810700: returns
// immediately for modes 0x12 and 0x15, otherwise builds a camera/transform rig.
// Caches arg0's +0x40/+0x48 floats (x/z), seeds an identity-ish matrix at
// D_700036A0 and a scaled matrix at D_700036E0 (0.8/1.0/0.5 diagonal consts),
// looks up a per-area base record via D_00265890[D_00810142] (0xC-byte stride,
// indexed again by D_00810144), reads its +8 float and combines with the global
// heading D_00810374 minus pi/2, wraps the angle through func_001B1470, composes
// D_700036A0 via func_001026D0/func_00102BB0, then builds a translate-only matrix
// at D_700038A0 from (256+z, 164-x), transforms it through func_001031E0, copies
// three lookup vectors D_0026A990/9A0/9B0 into D_700038A0/B0/C0 via func_00102948,
// rotates each through func_001026A0(., D_700036A0, .), rescales all three into
// screen/tile-space via the 16*(1792+v) / 16*(1936+v/2) per-axis formulas, then
// submits the packet via func_00207D00(1,3) and func_00208040(1, ..., 0x802040A0).
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_001026D0(void *dst, void *a, void *b);
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(void *m);
extern void func_00102BB0(void *dst, void *src, float angle);
extern void func_001031E0(void *dst, void *src);
extern float func_001B1470(float a);
extern void func_00207D00(int a, int b);
extern void func_00208040(int a, float *b, float *c, float *d, unsigned long long e);
extern float *D_00265890[];
extern float D_0026A990[2];
extern float D_0026A9A0[2];
extern float D_0026A9B0[2];
extern unsigned char D_00810142;
extern unsigned char D_00810144;
extern float D_00810374;
extern unsigned char D_00810700;
extern float D_700036A0[3];
extern float D_700036D0[3];
extern float D_700036E0[3];
extern float D_700038A0[4];
extern float D_700038B0[4];
extern float D_700038C0[4];

void func_00210F30(char *arg0) {
    float fx, fz;
    float *base;

    if (D_00810700 == 0x12 || D_00810700 == 0x15) {
        return;
    }
    fx = *(float *)(arg0 + 0x40);
    fz = *(float *)(arg0 + 0x48);
    func_001029C0(D_700036A0);
    func_001029C0(D_700036E0);
    *(int *)0x700036E0 = 0x3F4CCCCD;
    *(int *)0x700036F4 = 0x3F800000;
    *(int *)0x70003708 = 0x3F000000;
    *(int *)0x7000371C = 0;
    base = D_00265890[D_00810142];
    *(float *)0x70003A20 = (D_00810374 - base[D_00810144 * 3 + 2]) - 1.5707964f;
    *(float *)0x70003A20 = func_001B1470(*(float *)0x70003A20);
    func_001026D0(D_700036A0, D_700036E0, D_700036A0);
    func_00102BB0(D_700036A0, D_700036A0, *(float *)0x70003A20);
    D_700038A0[0] = 256.0f + fz;
    *(int *)0x700038A4 = 0;
    *(float *)0x700038A8 = 164.0f - fx;
    *(int *)0x700038AC = 0x3F800000;
    func_001031E0(D_700036D0, D_700038A0);
    func_00102948(D_700038A0, D_0026A990);
    func_00102948(D_700038B0, D_0026A9A0);
    func_00102948(D_700038C0, D_0026A9B0);
    func_001026A0(D_700038A0, D_700036A0, D_700038A0);
    func_001026A0(D_700038B0, D_700036A0, D_700038B0);
    func_001026A0(D_700038C0, D_700036A0, D_700038C0);
    D_700038A0[0] = 16.0f * (1792.0f + D_700038A0[0]);
    D_700038A0[2] = 16.0f * (1936.0f + (D_700038A0[2] / 2.0f));
    D_700038B0[0] = 16.0f * (1792.0f + D_700038B0[0]);
    D_700038C0[0] = 16.0f * (1792.0f + D_700038C0[0]);
    D_700038B0[2] = 16.0f * (1936.0f + (D_700038B0[2] / 2.0f));
    D_700038C0[2] = 16.0f * (1936.0f + (D_700038C0[2] / 2.0f));
    func_00207D00(1, 3);
    func_00208040(1, D_700038A0, D_700038B0, D_700038C0, (int)0x802040A0);
}
