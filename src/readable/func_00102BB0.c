// NEARMISS func_00102BB0  (vram 0x00102BB0, 0xA8 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 0.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written SDK VU0 code (an in-house sine / cosine series via func_001029E8 and a
// multiply-accumulate matrix product); the C states the same series, sign rule and product.
//
// The function links from the asm body in src/func_00102BB0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SDK VU0: rotates a 4x4 matrix about the y axis: dst = R * src row by
// row (each src row v becomes R.row0 * v.x + R.row1 * v.y + R.row2 * v.z +
// R.row3 * v.w), where R is the identity with the (2, 0) plane rotated by
// angle (cos on the diagonal, +sin / -sin off it). Hand-written in the
// original: the sine and cosine come from the SDK's own series in
// func_001029E8 (the angle is first folded against pi/2), and the matrix
// product is VU0 multiply-accumulate code. The C below states the series
// and the sign rule directly (sdk_sincos); it is not libm sinf / cosf.
// {c9, c7, c5, c3} of the SDK's odd sine polynomial (func_001029E8).
extern const float D_00241100[4];
extern float sqrtf(float x);   /* VU0 square root in the original */

// The original's sine / cosine, exactly as it computes them: the angle is
// folded against pi/2 (0x3FC90FDB): t = pi/2 + angle when angle < 0, else
// t = pi/2 - angle. func_001029E8 evaluates the odd series
// p = t + c3 t^3 + c5 t^5 + c7 t^7 + c9 t^9 (added in that order, each term
// formed as ((c * t) * u) * ... with u = t * t), which is sin(t) = cos(angle);
// the sine is sqrt(1 - p * p), negated when angle < 0. Valid domain:
// angle in [-pi, pi] (t in [-pi/2, pi/2]); outside it the series leaves its
// range and the sine keeps the sign of the angle rather than of sin(angle).
// The VU0 arithmetic is not IEEE (no denormals, truncating rounding).
static void sdk_sincos(float angle, float *s, float *c) {
    float t;
    float u;
    float p;
    int neg;

    if (angle < 0.0f) {
        t = 1.5707964f + angle;
        neg = 1;
    } else {
        t = 1.5707964f - angle;
        neg = 0;
    }
    u = t * t;
    p = t;
    p += D_00241100[3] * t * u;
    p += D_00241100[2] * t * u * u;
    p += D_00241100[1] * t * u * u * u;
    p += D_00241100[0] * t * u * u * u * u;
    *c = p;
    *s = sqrtf(1.0f - p * p);
    if (neg) {
        *s = -*s;
    }
}

void func_00102BB0(float *dst, float *src, float angle) {
    float r[16];
    float v[4];
    float s;
    float c;
    int i;
    int k;

    sdk_sincos(angle, &s, &c);

    for (i = 0; i < 16; i++) {
        r[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    }
    r[2 * 4 + 2] = c;
    r[2 * 4 + 0] = s;
    r[0 * 4 + 2] = -s;
    r[0 * 4 + 0] = c;
    for (k = 0; k < 4; k++) {
        for (i = 0; i < 4; i++) {
            v[i] = src[k * 4 + i];
        }
        for (i = 0; i < 4; i++) {
            dst[k * 4 + i] = r[i] * v[0] + r[4 + i] * v[1] + r[8 + i] * v[2] + r[12 + i] * v[3];
        }
    }
}
