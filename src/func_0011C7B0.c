// NEARMISS func_0011C7B0  (vram 0x0011C7B0, 0x3E0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.81% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// TWO issues. (1) TOOLCHAIN BUG (not a source wall): ee-gcc emits `li.s $fN,<const>` for SFmode constants; the expected object materializes them as `lui $at / ori $at / mtc1 $at,$fN`, which is what GNU `as` produces ONLY when `-G 0` is in effect. tools/eegcc/ee-compile.sh passes `-G0` to cc1 but NO...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern float func_0011DF78(float);                                        /* fabsf */
extern int func_0011CE20(float *, float *, int, int, int, const int *);   /* __kernel_rem_pio2f */

typedef union {
    float value;
    unsigned int word;
} ieee_float_shape_type;

extern const int D_0026C490[32];   /* npio2_hw[]   */
extern const int D_0026C178[];     /* two_over_pi[] */

static const float
zero    = 0.0f,                 /* 0x00000000 */
half    = 0.5f,                 /* 0x3f000000 */
two8    = 256.0f,               /* 0x43800000 */
invpio2 = 0.63661980628967285156f,    /* 0x3f22f984 */
pio2_1  = 1.57078552246093750000f,    /* 0x3fc90f80 */
pio2_1t = 1.08043341242591850460e-05f,    /* 0x37354443 */
pio2_2  = 1.08042731881141662598e-05f,    /* 0x37354400 */
pio2_2t = 6.07709993438732010000e-11f,    /* 0x2e85a308 */
pio2_3  = 6.07709438327219720000e-11f,    /* 0x2e85a300 */
pio2_3t = 6.12323426292583900000e-17f;    /* 0x248d3132 */

int func_0011C7B0(float x, float *y)
{
    float z, w, t, r, fn;
    float tx[3];
    int e0, i, j, nx, n, ix, hx;

    { ieee_float_shape_type gf_u; gf_u.value = x; hx = (int)gf_u.word; }
    ix = hx & 0x7fffffff;

    if (ix <= 0x3f490fd8) {             /* |x| ~<= pi/4: no reduction needed */
        y[0] = x;
        y[1] = 0;
        return 0;
    }

    if (ix < 0x4016cbe4) {              /* |x| ~< 3pi/4: n == +-1 */
        if (hx > 0) {
            z = x - pio2_1;
            if ((ix & 0xfffffff0) != 0x3fc90fd0) {   /* 24+24 bit pi is enough */
                y[0] = z - pio2_1t;
                y[1] = (z - y[0]) - pio2_1t;
            } else {                                 /* near pi/2: 24+24+24 bit pi */
                z -= pio2_2;
                y[0] = z - pio2_2t;
                y[1] = (z - y[0]) - pio2_2t;
            }
            return 1;
        } else {
            z = x + pio2_1;
            if ((ix & 0xfffffff0) != 0x3fc90fd0) {
                y[0] = z + pio2_1t;
                y[1] = (z - y[0]) + pio2_1t;
            } else {
                z += pio2_2;
                y[0] = z + pio2_2t;
                y[1] = (z - y[0]) + pio2_2t;
            }
            return -1;
        }
    }

    if (ix <= 0x43490f80) {             /* |x| ~<= 2^7*(pi/2): medium size */
        t = func_0011DF78(x);
        n = (int)(t * invpio2 + half);
        fn = (float)n;
        r = t - fn * pio2_1;
        w = fn * pio2_1t;               /* 1st round, good to 40 bits */
        if (n < 32 && (ix & 0xffffff00) != D_0026C490[n - 1]) {
            y[0] = r - w;               /* quick check: no cancellation */
        } else {
            unsigned int high;
            j = ix >> 23;
            { ieee_float_shape_type gf_u; gf_u.value = r - w; y[0] = gf_u.value; high = gf_u.word; }
            i = j - ((high >> 23) & 0xff);
            if (i > 8) {                /* 2nd iteration needed, good to 57 */
                t = r;
                w = fn * pio2_2;
                r = t - w;
                w = fn * pio2_2t - ((t - r) - w);
                { ieee_float_shape_type gf_u; gf_u.value = r - w; y[0] = gf_u.value; high = gf_u.word; }
                i = j - ((high >> 23) & 0xff);
                if (i > 25) {           /* 3rd iteration needed, 74 bits */
                    t = r;
                    w = fn * pio2_3;
                    r = t - w;
                    w = fn * pio2_3t - ((t - r) - w);
                    y[0] = r - w;
                }
            }
        }
        y[1] = (r - y[0]) - w;
        if (hx < 0) {
            y[0] = -y[0];
            y[1] = -y[1];
            return -n;
        }
        return n;
    }

    if (ix >= 0x7f800000) {             /* x is inf or NaN */
        y[0] = y[1] = x - x;
        return 0;
    }

    /* set z = scalbnf(|x|, ilogbf(x) - 7) */
    e0 = (ix >> 23) - 134;
    { ieee_float_shape_type sf_u; sf_u.word = (unsigned int)(ix - (e0 << 23)); z = sf_u.value; }
    for (i = 0; i < 2; i++) {
        tx[i] = (float)(int)z;
        z = (z - tx[i]) * two8;
    }
    tx[2] = z;
    nx = 3;
    while (tx[nx - 1] == zero)
        nx--;                           /* skip zero terms */
    n = func_0011CE20(tx, y, e0, nx, 2, D_0026C178);
    if (hx < 0) {
        y[0] = -y[0];
        y[1] = -y[1];
        return -n;
    }
    return n;
}
