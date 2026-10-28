// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 84.97% NEARMISS).
// Fixes read from the original instructions:
//  - the scratchpad vector at 0x700038A0 is one relocated extern float array:
//    its X/Y/Z components are read and written as D_700038A0[0..2], which lets
//    the second pass load each minuend in the original's order (the old
//    literal-address spelling made the compiler set up 13.0f first);
//  - the w component (1.0f bits) stays a plain literal-address int store.
//
// Builds a camera/anchor vector at the scratchpad block 0x700038A0..AC from the
// entity at arg0, then calls func_00102948 twice (copy/transform into D_008105E0
// then D_008105D0). func_0011DE90=cos, func_0011E2A8=sin. First pass: 38A0 =
// field(0xB0) - 8.0*cos(field(0xC4)); 38A4 = 10.0 + field(0xB4); 38A8 =
// field(0xB8) + 8.0*sin(field(0xC4)); 38AC = 1.0f bits. Second pass refines with
// the global angle D_00810374: 38A0 -= 13.0*sin(angle); 38A4 = 12.0 + field(0xB4);
// 38A8 -= 13.0*cos(angle).
extern float func_0011DE90(float a);   /* cos */
extern float func_0011E2A8(float a);   /* sin */
extern void func_00102948(void *dst, void *src);
extern float D_00810374;
extern char D_008105D0;
extern char D_008105E0;
extern float D_700038A0[4];

int func_001BBBF0(char *arg0) {
    D_700038A0[0] = *(float *)(arg0 + 0xB0) - 8.0f * func_0011DE90(*(float *)(arg0 + 0xC4));
    D_700038A0[1] = 10.0f + *(float *)(arg0 + 0xB4);
    D_700038A0[2] = *(float *)(arg0 + 0xB8) + 8.0f * func_0011E2A8(*(float *)(arg0 + 0xC4));
    *(int *)0x700038AC = 0x3F800000;
    func_00102948(&D_008105E0, D_700038A0);
    D_700038A0[0] = D_700038A0[0] - 13.0f * func_0011E2A8(D_00810374);
    D_700038A0[1] = 12.0f + *(float *)(arg0 + 0xB4);
    D_700038A0[2] = D_700038A0[2] - 13.0f * func_0011DE90(D_00810374);
    func_00102948(&D_008105D0, D_700038A0);
    return 1;
}
