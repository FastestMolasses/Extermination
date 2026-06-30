// NEARMISS func_0011D878  (vram 0x0011D878, 0x294 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 66.98% via ee-gcc 2.9-991111-01 (-O2 -G0). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc float-constant codegen wall. Function is fdlibm __kernel_tanf(float x,float y,int iy) (T[]=D_0026C598, pio4=0x3F490FDA, pio4lo=0x33222168, thr 0x31800000/0x3f2ca140) and was decompiled correctly. ee-gcc 2.9-991111-01 POOLS any float const whose low16 != 0 (emits lwc1 N(gp)), whereas the target materializes the...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2 -G0

extern float D_0026C598[];

extern float func_0011DF78(void);

float func_0011D878(float x, float y, int iy) {
    float z, r, v, w, s;
    int ix, hx;
    float *T = D_0026C598;

    hx = *(int *)&x;
    ix = hx & 0x7fffffff;

    if (ix < 0x31800000) {
        if ((int)x == 0) {
            if ((ix | (iy + 1)) == 0) {
                return 1.0f / func_0011DF78();
            } else {
                return (iy == 1) ? x : -1.0f / x;
            }
        }
    }

    if (ix >= 0x3f2ca140) {
        if (hx < 0) {
            x = -x;
            y = -y;
        }
        z = 7.8539812565e-01f - x;
        w = 3.7748947079e-08f - y;
        x = z + w;
        y = 0.0f;
    }

    z = x * x;
    w = z * z;
    r = T[1] + w * (T[3] + w * (T[5] + w * (T[7] + w * (T[9] + w * T[11]))));
    v = z * (T[2] + w * (T[4] + w * (T[6] + w * (T[8] + w * (T[10] + w * T[12])))));
    s = z * x;
    r = y + z * (s * (r + v) + y);
    r = r + T[0] * s;
    w = x + r;

    if (ix >= 0x3f2ca140) {
        v = (float)iy;
        return (float)(1 - ((hx >> 30) & 2)) *
               (v - (float)2.0 * (x - (w * w / (w + v) - r)));
    }

    if (iy == 1) {
        return w;
    } else {
        float a, t;
        int sw, st;
        sw = *(int *)&w;
        *(int *)&z = sw & 0xfffff000;
        v = r - (z - x);
        t = a = -1.0f / w;
        st = *(int *)&t;
        *(int *)&t = st & 0xfffff000;
        s = 1.0f + t * z;
        return t + a * (s + t * v);
    }
}
