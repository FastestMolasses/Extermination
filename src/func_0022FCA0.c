// NEARMISS func_0022FCA0  (vram 0x0022FCA0, 0x358 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.84% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring wall (target keeps the heading-error scratch value in caller-saved fv1/fv0f/ft0; mwcc233 promotes it to a callee-saved fs0), plus a bc1t/bc1f branch-sense scheduling difference on the facing-arc pick (both source polarities tried, no score change). Body/control-flow fully rec...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Turn-toward-target heading controller. Computes the yaw error (D_00810690 minus
// the actor's current facing at p+0xC, staged at scratchpad 0x70003A20). If the error
// is positive, snaps velocity toward the target via func_001028D0/func_00102760
// (rotation-table lookup into D_700038A0) scaled by the raw error. Otherwise picks a
// per-actor turn-rate threshold (-20 or -10 degrees depending on p+0x64's special
// case -46.8f), and if the error exceeds that threshold, either does the same
// rotation-table snap (when D_0081069C > 8.6, a speed/difficulty-style gate) or
// eases the heading incrementally: computes/caches the facing angle via
// func_001B1240 and a arc-side flag at p+3 (1 = turn positive, 2 = turn negative),
// nudges the cached angle by 0.3 degrees per unit of remaining error, and re-derives
// the velocity from sin/cos (func_0011E2A8/func_0011DE90) of the new angle. If the
// error doesn't exceed the threshold, clears the arc-side flag (idle).
extern void func_00102760(void *a, void *b);
extern void func_001028D0(void *dst, void *a, void *b);
extern float func_0011DE90(float);
extern float func_0011DF78(float);
extern float func_0011E2A8(float);
extern float func_001B1240(char *p, float a, float b);
extern float D_00810690;
extern float D_0081069C;
extern char D_700038A0[];

void func_0022FCA0(char *p) {
    float thresh;
    float ang;

    *(float *)0x70003A20 = D_00810690 - func_0011DF78(*(float *)(p + 0xC));
    if (*(float *)0x70003A20 > 0.0f) {
        func_001028D0(D_700038A0, p + 0x20, p + 0x10);
        *(int *)0x700038A4 = 0;
        *(int *)0x700038AC = 0;
        func_00102760(D_700038A0, D_700038A0);
        *(float *)(p + 0x10) = *(float *)(p + 0x10) + (*(float *)0x700038A0 * *(float *)0x70003A20);
        *(float *)(p + 0x18) = *(float *)(p + 0x18) + (*(float *)0x700038A8 * *(float *)0x70003A20);
        *(unsigned char *)(p + 3) = 0;
        return;
    }
    if (*(float *)(p + 0x64) == -46.8f) {
        thresh = -20.0f;
    } else {
        thresh = -10.0f;
    }
    if (*(float *)0x70003A20 < thresh) {
        *(float *)0x70003A24 = *(float *)0x70003A20 - thresh;
        if (D_0081069C > 8.6f) {
            func_001028D0(D_700038A0, p + 0x20, p + 0x10);
            *(int *)0x700038A4 = 0;
            *(int *)0x700038AC = 0;
            func_00102760(D_700038A0, D_700038A0);
            *(float *)(p + 0x10) = *(float *)(p + 0x10) + (*(float *)0x700038A0 * *(float *)0x70003A24);
            *(float *)(p + 0x18) = *(float *)(p + 0x18) + (*(float *)0x700038A8 * *(float *)0x70003A24);
            return;
        }
        if (*(unsigned char *)(p + 3) == 0) {
            *(float *)0x70003A28 = func_001B1240(p + 0x10, *(float *)(p + 0x20), *(float *)(p + 0x28));
            *(float *)0x70003A2C = *(float *)(p + 0x90) - *(float *)0x70003A28;
            if (*(float *)0x70003A2C <= 0.0f) {
                *(unsigned char *)(p + 3) = 2;
            } else {
                *(unsigned char *)(p + 3) = 1;
            }
        }
        if (*(unsigned char *)(p + 3) == 1) {
            ang = *(float *)0x70003A28 + ((3.1415927f * (0.3f * *(float *)0x70003A24)) / 180.0f);
        } else {
            ang = *(float *)0x70003A28 - ((3.1415927f * (0.3f * *(float *)0x70003A24)) / 180.0f);
        }
        *(float *)0x70003A28 = ang;
        *(float *)0x700038A0 = func_0011E2A8(*(float *)0x70003A28);
        *(int *)0x700038A4 = 0;
        *(float *)0x700038A8 = func_0011DE90(*(float *)0x70003A28);
        *(int *)0x700038AC = 0;
        *(float *)(p + 0x10) = *(float *)(p + 0x10) + (*(float *)0x700038A0 * *(float *)0x70003A24);
        *(float *)(p + 0x18) = *(float *)(p + 0x18) + (*(float *)0x700038A8 * *(float *)0x70003A24);
        return;
    }
    *(unsigned char *)(p + 3) = 0;
}
