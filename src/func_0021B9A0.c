// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// TU_STATIC: func_0021B900 func_0021B920
//
// Byte-matched (objdiff 100%) with func_0021B900 and func_0021B920 compiled as
// static functions of this file. The original compiler saw both earlier in the
// same translation unit and knew their register use: it keeps `mode` in the
// caller-saved a2 across the func_0021B920 call with a 0x10 frame that saves
// only ra (the former NEARMISS residual, 88.61%: a one-function file spills
// `mode` to a saved register). tools/decomp/tu_static.py removes the static
// copies after checking each equals the original callee; the calls link to the
// callees' own units.

//
// SEMANTICS: fog / depth-range programmer for the per-frame render context
// (*D_00275670).  Context fields +0xB8 / +0xBC hold the current fog NEAR and
// FAR distances (see func_0021B970, which writes the same pair verbatim);
// +0xD8 / +0xDC and +0xF8 / +0xFC hold two preset near/far pairs.
// `mode` selects how the new pair is produced:
//   1        -> preset pair at +0xD8 / +0xDC
//   2, 4     -> near = bias + near * scale, far unchanged
//   3, 5     -> near unchanged, far  = bias + far  * scale
//   0, other -> preset pair at +0xF8 / +0xFC
// The pair is stored back to +0xB8 / +0xBC and handed to func_0021B920, which
// derives the GS fog coefficients (255.0f / (far - near)) into +0xA0..+0xAC.
// For modes 0, 1, 4 and 5 func_0021B900 then latches +0xA0..+0xBF into +0xC0.

extern int *D_00275670;
static void func_0021B900(void);                 /* same-TU callees, see TU_STATIC */
static void func_0021B920(float f12, float f13);
#include "src/func_0021B900.c"
#include "src/func_0021B920.c"

void func_0021B9A0(int mode, float scale, float bias) {
    int *p;
    float near, far;

    switch (mode) {
    case 0:
    default:
        p = D_00275670;
        near = *(float *)((char *)p + 0xF8);
        far  = *(float *)((char *)p + 0xFC);
        break;
    case 1:
        p = D_00275670;
        near = *(float *)((char *)p + 0xD8);
        far  = *(float *)((char *)p + 0xDC);
        break;
    case 2:
    case 4:
        p = D_00275670;
        near = bias + *(float *)((char *)p + 0xB8) * scale;
        far  = *(float *)((char *)p + 0xBC);
        break;
    case 3:
    case 5:
        p = D_00275670;
        near = *(float *)((char *)p + 0xB8);
        far  = bias + *(float *)((char *)p + 0xBC) * scale;
        break;
    }
    p = D_00275670;
    *(float *)((char *)p + 0xB8) = near;
    p = D_00275670;
    *(float *)((char *)p + 0xBC) = far;
    func_0021B920(near, far);
    if (mode == 4 || mode == 5 || mode == 0 || mode == 1) {
        func_0021B900();
    }
}
