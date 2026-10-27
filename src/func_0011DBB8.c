// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.06% NEARMISS).
// Fixes read from the original instructions:
//  - The float word of x is read inside a do { ... } while (0) block, the fdlibm
//    GET_FLOAT_WORD macro shape (a union of float and int). ee-gcc records that block
//    as a loop, which aligns the first following label (the |x| >= 2^34, non-NaN
//    sign test) to 8 bytes; that is the padding after the NaN return, and the same
//    change moves the return-address save to the original prologue position.
//
// fdlibm atanf: func_0011DF78 is fabsf; D_0026C5F8 = aT[11], D_0026C5D8 = atanhi[4],
// D_0026C5E8 = atanlo[4], D_0026C5E4/F4 = atanhi[3]/atanlo[3]; huge = 1.0e30f.

extern float func_0011DF78(float);

extern float D_0026C5D8[];   /* atanhi[4] */
extern float D_0026C5E8[];   /* atanlo[4] */
extern float D_0026C5F8[];   /* aT[11]   */
extern float D_0026C5E4;     /* atanhi[3] */
extern float D_0026C5F4;     /* atanlo[3] */

float func_0011DBB8(float x)
{
    float w, s1, s2, z;
    int hx, ix, id;

    do { union { float f; int i; } gf_u; gf_u.f = x; hx = gf_u.i; } while (0);
    ix = hx & 0x7fffffff;

    if (ix > 0x507fffff) {
        if (ix > 0x7f800000)
            return x + x;
        if (hx > 0)
            return D_0026C5E4 + D_0026C5F4;
        else
            return -D_0026C5E4 - D_0026C5F4;
    }

    if (ix <= 0x3edfffff) {
        if (ix <= 0x30ffffff) {
            if (1.0f < x + 1.0e30f)
                return x;
        }
        id = -1;
    } else {
        x = func_0011DF78(x);
        if (ix <= 0x3f97ffff) {
            if (ix <= 0x3f2fffff) {
                id = 0;
                x = (2.0f * x - 1.0f) / (2.0f + x);
            } else {
                id = 1;
                x = (x - 1.0f) / (x + 1.0f);
            }
        } else {
            if (ix <= 0x401bffff) {
                id = 2;
                x = (x - 1.5f) / (1.0f + 1.5f * x);
            } else {
                id = 3;
                x = -1.0f / x;
            }
        }
    }

    z = x * x;
    w = z * z;
    s1 = z * (D_0026C5F8[0] + w * (D_0026C5F8[2] + w * (D_0026C5F8[4] +
              w * (D_0026C5F8[6] + w * (D_0026C5F8[8] + w * D_0026C5F8[10])))));
    s2 = w * (D_0026C5F8[1] + w * (D_0026C5F8[3] + w * (D_0026C5F8[5] +
              w * (D_0026C5F8[7] + w * D_0026C5F8[9]))));
    if (id < 0)
        return x - x * (s1 + s2);
    else {
        z = D_0026C5D8[id] - ((x * (s1 + s2) - D_0026C5E8[id]) - x);
        return (hx < 0) ? -z : z;
    }
}
