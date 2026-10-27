// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// SPAD: 0x700038A0 0x700038B0 0x700038C0

//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 99.87% NEARMISS).
// The C was already instruction-identical; the residual was relocation spelling
// (the original relocates the +8 element accesses against D_700038A0/B0/C0, splat
// leaves them as bare literals). tools/decomp/reloc_normalize.py now spells the
// target the way the compiled object does where both resolve to the same address.
//

//
// Body corrected earlier in the same wave (80.03 -> 99.87); the old
// "stack-packing wall" and "scheduler wall" verdicts were our bugs:
//  - the two heading vectors are float[4] (16-byte) stack buffers whose y
//    element is cleared, not float[3] plus a separate int; declared before
//    the 4x4 scratch matrix, which gives the original's frame layout;
//  - func_00208040 takes a fifth argument (its definition in src/), passed
//    the sign-extended constant 0x8000FFFF;
//  - the three scratchpad corner vectors are array externs accessed by
//    index; that lets mwcc know they do not alias, reproducing the original
//    interleaved rescale schedule (integer-literal addresses did not).
//
// Radar/HUD camera-relative setup. arg0 is the viewer entity. Dispatches
// func_00207D00(1,0), then runs func_0022E630(arg0, blip) over the live blip
// list (D_00275B58 count / D_00275B54 array, skipping empty-string entries).
// Builds a scratch transform D_700036E0 (0.8/1.0/0.5/0 packed floats), computes
// the signed heading delta between arg0's forward vector and a fixed +240
// offset vector via func_0011E620 (atan2-like) normalized into (-pi,pi], turns
// that into a yaw-only rotation matrix (func_001029C0/func_00102BB0), combines
// it into D_700036A0, seeds a camera-frustum quad (96,0,340,1.0) transformed by
// func_001031E0, copies three D_0026A9x0 corner vectors into D_700038Ax0,
// rotates them by D_700036A0 (func_001026A0), rescales x and z of each by
// 16*(1792+x) / 16*(1936+z/2), and finally hands the block to func_00208040
// plus three func_002082B0(1, D_0026Axxxx) HUD-icon calls.
extern void func_00207D00(int a, int b);
extern void func_0022E630(char *arg0, char *arg1);
extern void func_001029C0(void *mtx);
extern void func_00102948(void *dst, void *src);
extern void func_00102760(void *dst, void *src);
extern float func_0011E620(float dx, float dy);
extern void func_00102BB0(void *dst, void *src, float angle);
extern void func_001026D0(void *dst, void *a, void *b);
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_001031E0(void *dst, void *src);
extern void func_00208040(int a, float *b, float *c, float *d, unsigned long long e);
extern void func_002082B0(int a, void *b);

extern short D_00275B58;
extern char **D_00275B54;
extern float D_0026A990[2];
extern float D_0026A9A0[2];
extern float D_0026A9B0[2];
extern char D_0026A9C0[8];
extern char D_0026AA20[8];
extern char D_0026AA80[8];
extern float D_700036A0[3];
extern float D_700036D0[3];
extern float D_700036E0[3];
extern float D_700038A0[4];
extern float D_700038B0[4];
extern float D_700038C0[4];

void func_0022E7F0(char *arg0) {
    int cnt;
    char **list;
    float ang0;
    float ang1;
    float da;
    float sp50[4]; /* heading vectors; [1] (y) is cleared to flatten them */
    float sp60[4];
    float sp70[16];

    func_00207D00(1, 0);

    cnt = D_00275B58;
    list = D_00275B54;
    if (cnt != 0) {
        do {
            char *e = *list;
            cnt--;
            list++;
            if (*(unsigned char *)e != 0) {
                func_0022E630(arg0, e);
            }
        } while (cnt != 0);
    }

    func_001029C0(D_700036E0);
    *(int *)0x700036E0 = 0x3F4CCCCD;
    *(int *)0x700036F4 = 0x3F800000;
    *(int *)0x70003708 = 0x3F000000;
    *(int *)0x7000371C = 0;

    func_00102948(sp60, (char *)*(int *)(arg0 + 0x12C) + 0x90);
    sp60[1] = 0.0f;
    func_00102760(sp60, sp60);
    ang0 = func_0011E620(sp60[2], sp60[0]);

    func_00102948(sp50, arg0 + 0xF0);
    sp50[1] = 0.0f;
    func_00102760(sp50, sp50);
    ang1 = func_0011E620(sp50[2], sp50[0]);

    da = -(ang0 - ang1);
    if (da <= -3.1415927f) {
        do {
            da += 6.2831855f;
        } while (da <= -3.1415927f);
    }
    if (!(da <= 3.1415927f)) {
        do {
            da = da - 6.2831855f;
        } while (!(da <= 3.1415927f));
    }

    func_001029C0(sp70);
    func_00102BB0(sp70, sp70, da);
    func_001026D0(D_700036E0, sp70, D_700036E0);

    func_001029C0(D_700036A0);
    func_001026D0(D_700036A0, D_700036E0, D_700036A0);

    D_700038A0[0] = 96.0f;
    *(int *)0x700038A4 = 0;
    *(float *)0x700038A8 = 340.0f;
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

    func_00208040(1, D_700038A0, D_700038B0, D_700038C0, (int)0x8000FFFF);
    func_002082B0(1, D_0026A9C0);
    func_002082B0(1, D_0026AA20);
    func_002082B0(1, D_0026AA80);
}
