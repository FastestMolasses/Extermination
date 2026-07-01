// NEARMISS func_001B62C0  (vram 0x001B62C0, 0x228 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.84% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// CW-vs-mwcc branch lowering of the 4-way octant selector. The mula.s/madd.s MAC pair for dx*dx+dy*dy WAS reproduced (matched), disproving an FPU-MAC wall here. The residual: the target branches (bc1t) forward to two SHARED trig-call sites (cos at 0x63FC, sin at 0x6410) reached from multiple condit...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 88.84% (mwcc233). Analog-stick -> aim-vector converter.
// D_00810E64/E65 are two raw (0..255) analog-axis bytes; dx/dy re-center to
// [-128,127]. mag = sqrt(dx^2+dy^2) (mula.s/madd.s MAC pair reproduced);
// ang = atan2(dy,dx). An octant test on ang (against +-pi/4, +-3pi/4)
// selects cos vs sin of ang for the deadzone-scale term t=c*128; the scale
// is mag/sqrt(16384+t^2) clamped: <=0.25 -> 0, else 1.53846*(x-0.25) capped 1.
// out[0]=scl*cos(ang), out[1]=scl*sin(ang), out[2]=scl, out[3]=ang.
// RESIDUAL WALL: CW-vs-mwcc branch lowering of the 4-way octant select --
// the target branches (bc1t) forward to two SHARED trig-call sites; mwcc233
// lays each call inline and skips with bc1f. No source shape flips this.
// Secondary: commutative mul.s operand-coloring on dx*dx+dy*dy and scl*.
extern float func_0011DE90(float a); /* cos */
extern float func_0011E2A8(float a); /* sin */
extern float func_0011E620(float dy, float dx); /* atan2 */
extern float func_0011E748(float a); /* sqrt */

extern unsigned char D_00810E64;
extern unsigned char D_00810E65;

void func_001B62C0(float *out) {
    float dx, dy, mag, ang, t, c, scl;

    dx = (float)(unsigned int)D_00810E64 - 128.0f;
    dy = (float)(unsigned int)D_00810E65 - 128.0f;
    mag = func_0011E748(dx * dx + dy * dy);
    ang = func_0011E620(dy, dx);

    if (0.7853981852531433f <= ang && ang < 2.356194496154785f) {
        c = func_0011DE90(ang);
    } else if (-0.7853981852531433f < ang) {
        c = func_0011E2A8(ang);
    } else if (ang <= -2.356194496154785f) {
        c = func_0011E2A8(ang);
    } else {
        c = func_0011DE90(ang);
    }

    t = c * 128.0f;
    mag = mag / func_0011E748(16384.0f + t * t);
    if (mag <= 0.25f) {
        scl = 0.0f;
    } else {
        scl = 1.5384615659713745f * (mag - 0.25f);
        if (scl > 1.0f) {
            scl = 1.0f;
        }
    }

    out[0] = scl * func_0011DE90(ang);
    out[1] = scl * func_0011E2A8(ang);
    out[2] = scl;
    out[3] = ang;
}
