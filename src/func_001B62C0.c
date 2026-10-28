// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 88.84% NEARMISS).
// Fixes read from the original instructions:
// - The octant select is ONE condition with two call sites: cos when ang is in
//   [pi/4, 3pi/4) or in (-3pi/4, -pi/4], else sin. The old if/else ladder with
//   four arms (two duplicate cos/sin calls) laid each call out inline; the
//   "shared trig-call site" wall was the wrong control-flow shape.
// - The deadzone term is scaled in place (`c *= 128.0f; mag /= sqrt(...)`),
//   and the ramp subtracts first into scl, then multiplies; both decide the
//   temporary float register colouring.

// Analog-stick -> aim-vector converter.
// D_00810E64/E65 are two raw (0..255) analog-axis bytes; dx/dy re-center to
// [-128,127]. mag = sqrt(dx^2+dy^2); ang = atan2(dy,dx). An octant test on ang
// (against +-pi/4, +-3pi/4) selects cos vs sin of ang for the deadzone-scale
// term t=c*128; the scale is mag/sqrt(16384+t^2) clamped: <=0.25 -> 0, else
// 1.53846*(x-0.25) capped 1.
// out[0]=scl*cos(ang), out[1]=scl*sin(ang), out[2]=scl, out[3]=ang.
extern float func_0011DE90(float a); /* cos */
extern float func_0011E2A8(float a); /* sin */
extern float func_0011E620(float dy, float dx); /* atan2 */
extern float func_0011E748(float a); /* sqrt */

extern unsigned char D_00810E64;
extern unsigned char D_00810E65;

void func_001B62C0(float *out) {
    float dx, dy, mag, ang, c, scl;

    dx = (float)(unsigned int)D_00810E64 - 128.0f;
    dy = (float)(unsigned int)D_00810E65 - 128.0f;
    mag = func_0011E748(dx * dx + dy * dy);
    ang = func_0011E620(dy, dx);

    if ((0.7853981852531433f <= ang && ang < 2.356194496154785f) || !(-0.7853981852531433f < ang || ang <= -2.356194496154785f)) {
        c = func_0011DE90(ang);
    } else {
        c = func_0011E2A8(ang);
    }

    c *= 128.0f;
    mag /= func_0011E748(16384.0f + c * c);
    if (mag <= 0.25f) {
        scl = 0.0f;
    } else {
        scl = mag - 0.25f;
        scl = 1.5384615659713745f * scl;
        if (scl > 1.0f) {
            scl = 1.0f;
        }
    }

    out[0] = scl * func_0011DE90(ang);
    out[1] = scl * func_0011E2A8(ang);
    out[2] = scl;
    out[3] = ang;
}
