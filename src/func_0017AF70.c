// NEARMISS func_0017AF70  (vram 0x0017AF70, 0x388 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.23% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// see summary
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Turret/limb aim-tracking blend step, gated on the linked-entity flag at
// arg0+0x2F2. Picks a pair of yaw/pitch step-rate constants based on the
// mode byte at arg0+5 (0x1D vs 0x1E vs other), fetches the target position
// via func_00183C40(D_008106E0, &tmp), then computes the desired yaw delta
// (func_001B1240 -> angle, func_001B1470 normalizes vs arg0+0xC4) and blends
// arg0+0x27C toward it by a fraction of the delta scaled by the appropriate
// rate constant, clamped to [0,1]. Repeats for pitch using arg0+0x278 vs
// arg0+0xC4 baseline through func_0017A800/func_0011E748 (sqrt of sum of
// squares). Stages the resulting delta into D_700038A0/A4 with
// D_700038A8=0, D_700038AC=1.0f and calls func_00102760 to normalize it,
// then measures the normalized magnitude (sum of squares via mula.s/
// madd.s) against 0.02f: if small enough, snaps arg0+0x27C/0x278 straight
// to the blended targets; otherwise nudges them by 0.02f times the
// normalized delta components.
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
extern float D_700038A4;
extern int D_700038A8;
extern int D_700038AC;
extern float D_700038B0;
extern float D_700038B4;
extern float D_70003A20;

void func_0017AF70(char *arg0) {
    float tmp[2];
    float yawRate, pitchUpRate, pitchDownRate, r21, r22, r23;
    float yaw, pitch;
    float mag;
    float a, b, cur;

    if (*(unsigned char *)(arg0 + 0x2F2) == 0) {
        return;
    }

    if (*(unsigned char *)(arg0 + 5) == 0x1D || *(unsigned char *)(arg0 + 5) == 0x1E) {
        yawRate = 0.000378f;
        pitchUpRate = 1.046904f;
        pitchDownRate = -1.047029f;
        r21 = 1.569371f;
        r22 = 1.395721f;
        r23 = 1.397294f;
    } else {
        yawRate = 0.000594f;
        pitchUpRate = 1.045836f;
        pitchDownRate = -1.046307f;
        r21 = 1.565551f;
        r22 = 1.393529f;
        r23 = 1.3981009f;
    }
    /* NOTE: the target's if/else structure is: if(mode!=0x1D){ if(mode==0x1E) use-0x1D-set;
       else use-0x1E-set } else use-0x1D-set -- i.e. mode 0x1D and 0x1E share constants,
       "other" gets the alternate set. The condition above (mode==0x1D || mode==0x1E) matches
       that grouping. */

    func_00183C40(D_008106E0, tmp);

    a = func_001B1470(func_001B1240(*(char **)(arg0 + 0x20) + 0xA0, tmp[0], tmp[1]) - *(float *)(arg0 + 0xC4));
    b = func_001B1470(func_0011E620(*(float *)(*(char **)(arg0 + 0x20) + 0xC0), *(float *)(*(char **)(arg0 + 0x20) + 0xC8)) - *(float *)(arg0 + 0xC4));

    if (a <= yawRate) {
        yaw = *(float *)(arg0 + 0x27C) - (0.5f * ((a - b) / pitchDownRate));
    } else {
        yaw = *(float *)(arg0 + 0x27C) + (0.5f * ((a - b) / pitchUpRate));
    }
    cur = 0.0f;
    if (yaw < 0.0f) {
        yaw = cur;
    } else {
        cur = 1.0f;
        if (yaw <= 1.0f) {
            cur = yaw;
        }
        yaw = cur;
    }

    a = func_0017A800(*(char **)(arg0 + 0x20) + 0xA0, tmp);
    b = func_001B1510(func_0011E748((*(float *)(*(char **)(arg0 + 0x20) + 0xC0) * *(float *)(*(char **)(arg0 + 0x20) + 0xC0)) + (*(float *)(*(char **)(arg0 + 0x20) + 0xC8) * *(float *)(*(char **)(arg0 + 0x20) + 0xC8))) - *(float *)(arg0 + 0xC4));

    if (a <= r21) {
        pitch = *(float *)(arg0 + 0x278) - (0.5f * ((a - b) / r23));
    } else {
        pitch = *(float *)(arg0 + 0x278) + (0.5f * ((b - a) / r22));
    }
    cur = 0.0f;
    if (pitch < 0.0f) {
        pitch = cur;
    } else {
        cur = 1.0f;
        if (pitch <= 1.0f) {
            cur = pitch;
        }
        pitch = cur;
    }

    D_700038A0 = yaw - *(float *)(arg0 + 0x27C);
    D_700038A4 = pitch - *(float *)(arg0 + 0x278);
    D_700038A8 = 0;
    D_700038AC = 0x3F800000;
    func_00102760(&D_700038B0, &D_700038A0);

    mag = func_0011E748((D_700038A0 * D_700038A0) + (D_700038A4 * D_700038A4));
    D_70003A20 = mag;
    if (mag <= 0.02f) {
        *(float *)(arg0 + 0x27C) = yaw;
        *(float *)(arg0 + 0x278) = pitch;
    } else {
        *(float *)(arg0 + 0x27C) += 0.02f * D_700038B0;
        *(float *)(arg0 + 0x278) += 0.02f * D_700038B4;
    }
}
