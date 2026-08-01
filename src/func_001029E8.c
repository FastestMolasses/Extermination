// NEARMISS func_001029E8  (vram 0x001029E8, 0x74 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 0.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef struct { float x, y, z, w; } VuVec;

/* {c9, c7, c5, c3} of the odd sine polynomial. */
extern const VuVec D_00241100;

/* VU0 macro-mode register file, shared with this function's callers. */
extern VuVec vf4, vf5, vf6;
extern float vu0_sqrt(float);   /* vsqrt Q, ... ; vwaitq */

void func_001029E8(int negate_cos)
{
    const VuVec *c = &D_00241100;
    float t, u, s, co;

    t = vf6.x;              /* vmr32.w keeps a copy of t in vf6.w   */
    u = t * t;              /* vmul.x vf6, vf6, vf6 -> vf6.x = t^2  */

    /* vf8 = c * t, then progressively narrower lane masks are scaled by u,
       so lane w reaches t^3, lane z t^5, lane y t^7 and lane x t^9. The
       accumulation order below is the order of the four vadd*.x ops. */
    s  = t;                             /* vaddx.x vf4, vf0, vf6x */
    s += c->w * t * u;                  /* + c3 * t^3 */
    s += c->z * t * u * u;              /* + c5 * t^5 */
    s += c->y * t * u * u * u;          /* + c7 * t^7 */
    s += c->x * t * u * u * u * u;      /* + c9 * t^9 */

    vf5.x = vf5.y = vf5.z = vf5.w = 0.0f;   /* vsub.xyzw vf5, vf0, vf0 */

    co = vu0_sqrt(1.0f - s * s);        /* cos(t); t is in [0, pi/2] */

    vf4.x = negate_cos ? -co : co;
    vf4.y = s;
    vf4.z = 0.0f;
    vf4.w = 0.0f;
}
