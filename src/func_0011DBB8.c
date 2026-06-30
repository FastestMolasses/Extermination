// NEARMISS func_0011DBB8  (vram 0x0011DBB8, 0x2A8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.41% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc constant-pool / list-scheduler wall. Function fully decompiled as fdlibm atanf (verified: func_0011DF78 is fabsf; tables D_0026C5F8=aT[11], D_0026C5E8=atanhi[4], D_0026C5D8=atanlo[4], D_0026C5E4/F4=atanhi[3]/atanlo[3]; huge=1.0e30f). Body + all branch-block ordering + polynomial Horner chains + range-reduction...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern float func_0011DF78(float);

extern float D_0026C5D8[];   /* atanlo[4] */
extern float D_0026C5E8[];   /* atanhi[4] */
extern float D_0026C5F8[];   /* aT[11]   */
extern float D_0026C5E4;     /* atanhi[3] */
extern float D_0026C5F4;     /* atanlo[3] */

float func_0011DBB8(float x)
{
    float w, s1, s2, z;
    int hx = *(int *)&x;
    int ix = hx & 0x7fffffff;
    int id;

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
        z = D_0026C5E8[id] - ((x * (s1 + s2) - D_0026C5D8[id]) - x);
        return (hx < 0) ? -z : z;
    }
}
