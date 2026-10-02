// NEARMISS func_0011C128  (vram 0x0011C128, 0x39C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 87.62% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc register allocation and the schedule of the polynomial
// and of the split (|x| between 0.5 and 0.975) branch; constants, branch structure and
// the operations follow the instructions.
//
// The function links from the asm body in src/func_0011C128.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. Later-level decomp lane (LDEC) 2026-10-01 (docs/LEVELS_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// asinf (single precision, newlib style), the tiny-|x| path included. With ix =
// |x|'s bits: ix == 1.0 gives x * pio2_hi + x * pio2_lo; ix > 1.0 gives (x - x) /
// (x - x); |x| < 0.5: for |x| < 2^-27 it returns x when 1e30 + x > 1.0 (otherwise it
// falls through with t unset, as the original does), else t = x * x and the result is
// x + x * p(t) / q(t). Otherwise w = 1 - |x| (func_0011DF78), t = w * 0.5, s =
// sqrtf(t) (func_0011CB90); |x| >= 0.975: pio2_hi - (2 (s + s p/q) - pio2_lo); else
// the high-part split with df = s's bits & 0xFFFFF000. The result is negated unless x
// is positive (as a signed word).
typedef union {
    float f;
    int i;
} FloatBits;

extern float func_0011DF78(float x);
extern float func_0011CB90(float x);

float func_0011C128(float x) {
    FloatBits u;
    int hx;
    int ix;
    float t, w, p, q, s, c, r;
    u.f = x;
    hx = u.i;
    ix = hx & 0x7FFFFFFF;
    if (ix == 0x3F800000) {
        return x * 1.5707962513e+00f + x * 7.5497894159e-08f;
    } else if (ix > 0x3F800000) {
        return (x - x) / (x - x);
    } else if (ix < 0x3F000000) {
        if (ix < 0x32000000) {
            if (1.0e30f + x > 1.0f) {
                return x;
            }
        } else {
            t = x * x;
        }
        p = t * (1.6666667163e-01f + t * (-3.2556581497e-01f + t * (2.0121252537e-01f +
            t * (-4.0055535734e-02f + t * (7.9153501429e-04f + t * 3.4793309169e-05f)))));
        q = 1.0f + t * (-2.4033949375e+00f + t * (2.0209457874e+00f + t * (-6.8828397989e-01f +
            t * 7.7038154006e-02f)));
        w = p / q;
        return x + x * w;
    }
    w = 1.0f - func_0011DF78(x);
    t = w * 0.5f;
    p = t * (1.6666667163e-01f + t * (-3.2556581497e-01f + t * (2.0121252537e-01f +
        t * (-4.0055535734e-02f + t * (7.9153501429e-04f + t * 3.4793309169e-05f)))));
    q = 1.0f + t * (-2.4033949375e+00f + t * (2.0209457874e+00f + t * (-6.8828397989e-01f +
        t * 7.7038154006e-02f)));
    s = func_0011CB90(t);
    if (ix >= 0x3F79999A) {
        w = p / q;
        t = 1.5707962513e+00f - (2.0f * (s + s * w) - 7.5497894159e-08f);
    } else {
        FloatBits d;
        d.f = s;
        d.i &= 0xFFFFF000;
        w = d.f;
        c = (t - w * w) / (s + w);
        r = p / q;
        p = 2.0f * s * r - (7.5497894159e-08f - 2.0f * c);
        q = 7.8539818525e-01f - 2.0f * w;
        t = 7.8539818525e-01f - (p - q);
    }
    if (hx > 0) {
        return t;
    } else {
        return -t;
    }
}
