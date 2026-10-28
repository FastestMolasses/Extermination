// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 87.23% NEARMISS).
// Fixes read from the original instructions:
//  - the blend arms were swapped: above the threshold the (a - b) delta is divided
//    by the positive rate and SUBTRACTED; at or below it, divided by the negative
//    rate and added (pitch: above -> (a - b) / hi subtracted, else (b - a) / lo added).
//  - the second distance call is atan2-like func_0011E620(sqrt(x*x + z*z), link+0xC4)
//    on the LINKED entity, then func_001B1510; arg0+0xC4 is not subtracted there.
//  - the target point buffer is a 3-float vector (x at [0], z at [2]).
//  - the delta quotient is its own statement before the field load and scale.
//  - clamps are plain if / else-if; the scratchpad vector is written and read
//    through literal addresses (only the call arguments are named symbols);
//    the final test is `mag > 0.02f` with the nudge arm first.
//  - local declaration order (yaw, pitch first) sets the FP saved registers.
//
// Turret/limb aim-tracking blend step, gated on the linked-entity flag at
// arg0+0x2F2. Picks yaw/pitch threshold and rate constants based on the mode
// byte at arg0+5 (0x1D and 0x1E share one set, other modes the alternate set),
// fetches the target position via func_00183C40(D_008106E0, &tmp), computes the
// yaw angle to it and the linked entity's current yaw (both normalized by
// func_001B1470 against arg0+0xC4), and blends arg0+0x27C by half the delta over
// the selected rate, clamped to [0,1]. Repeats for pitch (func_0017A800 vs the
// linked entity's horizontal length/height angle) into arg0+0x278. Stages the
// resulting delta into the scratchpad vector at 0x700038A0 (w = 1.0f),
// normalizes it into 0x700038B0 via func_00102760, and if its length exceeds
// 0.02f nudges both fields by 0.02f times the normalized components, else snaps
// them to the blended targets. The length is also stored to D_70003A20.
extern void func_00102760(void *dst, void *src);
extern float func_0011E620(float, float);
extern float func_0011E748(float x);
extern float func_0017A800(void *a0, float *a1);
extern void func_00183C40(int a0, float *a1);
extern float func_001B1240(void *p, float a, float b);
extern float func_001B1470(float v);
extern float func_001B1510(float x);

extern int D_008106E0;
extern float D_700038A0;
extern float D_700038B0;
extern float D_70003A20;

void func_0017AF70(char *arg0) {
    float tmp[3];
    float yaw, pitch;
    float yawRate, pitchRate, pitchDivLo, pitchDivHi, yawDivHi, yawDivLo;
    float mag;
    float a, b;
    char *link;

    if (*(unsigned char *)(arg0 + 0x2F2) == 0) {
        return;
    }

    if (*(unsigned char *)(arg0 + 5) == 0x1D || *(unsigned char *)(arg0 + 5) == 0x1E) {
        yawRate = 0.000378f;
        yawDivHi = 1.046904f;
        yawDivLo = -1.047029f;
        pitchRate = 1.569371f;
        pitchDivLo = 1.395721f;
        pitchDivHi = 1.397294f;
    } else {
        yawRate = 0.000594f;
        yawDivHi = 1.045836f;
        yawDivLo = -1.046307f;
        pitchRate = 1.565551f;
        pitchDivLo = 1.393529f;
        pitchDivHi = 1.3981009f;
    }

    func_00183C40(D_008106E0, tmp);

    a = func_001B1470(func_001B1240(*(char **)(arg0 + 0x20) + 0xA0, tmp[0], tmp[2]) - *(float *)(arg0 + 0xC4));
    b = func_001B1470(func_0011E620(*(float *)(*(char **)(arg0 + 0x20) + 0xC0), *(float *)(*(char **)(arg0 + 0x20) + 0xC8)) - *(float *)(arg0 + 0xC4));

    if (a > yawRate) {
        yaw = (a - b) / yawDivHi;
        yaw = *(float *)(arg0 + 0x27C) - 0.5f * yaw;
    } else {
        yaw = (a - b) / yawDivLo;
        yaw = *(float *)(arg0 + 0x27C) + 0.5f * yaw;
    }
    if (yaw < 0.0f) {
        yaw = 0.0f;
    } else if (yaw > 1.0f) {
        yaw = 1.0f;
    }

    a = func_0017A800(*(char **)(arg0 + 0x20) + 0xA0, tmp);
    link = *(char **)(arg0 + 0x20);
    b = func_0011E748(*(float *)(link + 0xC0) * *(float *)(link + 0xC0) + *(float *)(link + 0xC8) * *(float *)(link + 0xC8));
    b = func_001B1510(func_0011E620(b, *(float *)(*(char **)(arg0 + 0x20) + 0xC4)));

    if (a > pitchRate) {
        pitch = (a - b) / pitchDivHi;
        pitch = *(float *)(arg0 + 0x278) - 0.5f * pitch;
    } else {
        pitch = (b - a) / pitchDivLo;
        pitch = *(float *)(arg0 + 0x278) + 0.5f * pitch;
    }
    if (pitch < 0.0f) {
        pitch = 0.0f;
    } else if (pitch > 1.0f) {
        pitch = 1.0f;
    }

    *(float *)0x700038A0 = yaw - *(float *)(arg0 + 0x27C);
    *(float *)0x700038A4 = pitch - *(float *)(arg0 + 0x278);
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    func_00102760(&D_700038B0, &D_700038A0);

    mag = func_0011E748(*(float *)0x700038A0 * *(float *)0x700038A0 + *(float *)0x700038A4 * *(float *)0x700038A4);
    D_70003A20 = mag;
    if (mag > 0.02f) {
        *(float *)(arg0 + 0x27C) += 0.02f * *(float *)0x700038B0;
        *(float *)(arg0 + 0x278) += 0.02f * *(float *)0x700038B4;
    } else {
        *(float *)(arg0 + 0x27C) = yaw;
        *(float *)(arg0 + 0x278) = pitch;
    }
}
