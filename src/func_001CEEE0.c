// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 83.44% NEARMISS).
// Fixes read from the original instructions:
//  - The globals D_002512E8..D_00251360 are fields of ONE struct at D_002512E0 (the
//    same block passed to func_001CFBE0, which reads its mode word at +0x8C); writing
//    them as fields of one object gives the original store schedule (the arg3 store
//    stays late) and the 128-bit copies land in the incoming pointer registers.
//  - The {0, 0, scale, 1.0f} stack quad is a struct of int/float fields, so its stores
//    address the stack directly instead of through computed pointers.
//  - arg0 is a pointer (func_001026A0 and func_001CFA60 take pointers there).
//
// Stashes the two incoming 128-bit vectors to D_002512E0+0x20/+0x30, the scale float to
// +0x08, fparg1 to +0x40/+0x44, fparg2 to +0x50/+0x54 and arg3 to +0x80; builds a
// {0, 0, scale, 1.0f} quad on the stack and runs func_001026A0 on it, gets a handle via
// func_001CCF70(quad), fills a 0x60-byte block via func_001CFA60(block, arg0, 1.0f, 0.1f),
// then func_001CFBE0(handle, 1, &D_002512E0, block, 1).
typedef unsigned __int128 uint128;

extern void func_001026A0(void *a0, void *a1, void *a2);
extern int func_001CCF70(void *a0);
extern void func_001CFA60(void *obj, void *src, float f12, float f13);
extern void func_001CFBE0(int id, unsigned int kind, void *st, void *xf, int copy);

struct EffectState {
    int pad00[2];
    float scale;            /* +0x08 */
    char pad0C[0x20 - 0x0C];
    uint128 v20;            /* +0x20 */
    uint128 v30;            /* +0x30 */
    float f40;              /* +0x40 */
    float f44;              /* +0x44 */
    char pad48[0x50 - 0x48];
    float f50;              /* +0x50 */
    float f54;              /* +0x54 */
    char pad58[0x80 - 0x58];
    int w80;                /* +0x80 */
    char pad84[0x8C - 0x84];
    int mode;               /* +0x8C */
};
extern struct EffectState D_002512E0;

void func_001CEEE0(void *arg0, uint128 *arg1, uint128 *arg2, int arg3, float fparg0, float fparg1, float fparg2) {
    struct { int x; int y; float z; float w; } v;  /* {0, 0, scale, 1.0f} */
    char sp40[0x60];
    int r;

    *(uint128 *)&D_002512E0.v20 = *arg1;
    *(uint128 *)&D_002512E0.v30 = *arg2;
    D_002512E0.scale = fparg0;
    D_002512E0.f40 = fparg1;
    D_002512E0.f44 = fparg1;
    D_002512E0.f50 = fparg2;
    v.x = 0;
    D_002512E0.f54 = fparg2;
    v.y = 0;
    v.z = fparg0;
    D_002512E0.w80 = arg3;
    v.w = 1.0f;
    func_001026A0(&v, arg0, &v);
    r = func_001CCF70(&v);
    func_001CFA60(sp40, arg0, 1.0f, 0.1f);
    func_001CFBE0(r, 1, &D_002512E0, sp40, 1);
}
