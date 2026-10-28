// COMPILER: mwcc233
// SPAD: 0x700038A0 0x700038A4 0x700038A8 0x700038AC 0x700038B0 0x700038B8
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 92.50% NEARMISS).
// Fixes read from the original instructions:
//  - every switch case ends in break (not return): this restores the unfilled
//    dispatch slot and the duplicated constant at each spin-sign else arm;
//  - the rig at arg0+0x1F0 is indexed as an array of 4x4 float matrices, so
//    its base is recomputed per use (copy_qw4 targets [0]/[1]; the spin
//    speed/angle/timer are row 3 of matrix [3]); the speed pointer is taken
//    before the angle pointer and the timer pointer after the first lookup,
//    and the timer is declared before the speed (saved-register order);
//  - the angle advance is written angle += speed;
//  - scratchpad words are relocated volatile float externs (idiom-32, // SPAD:),
//    so the D_00275B40 load can move above the 0x700038A0 store.
//
// 4-state look-at/camera-spin driver dispatched on the state byte at arg0+4
// (gp-relative D_00275B40 holds the two active camera/root pointers [0] and [1];
// the 0x7000xxxx addresses are scratchpad vectors).
// State 0: once func_001B0FD0(arg0) returns 0, set arg0[0]=1, call func_001C6380,
// snapshot the two camera roots (+0x90) into rig matrices [0] and [1] via
// copy_qw4, advance to state 4. State 4: if the arg0+0x36 trigger is set, enter
// state 1, reset the spin params (timer 0x2E8 = 0.2, angle 0x2E4 = 0), rebuild
// the orientation (func_001028D0/func_001029C0/func_00102BB0/func_001026A0) and
// pick the spin speed sign (+/-0.08 at 0x2E0) from the staged vector at
// 0x700038A0; then run func_001B17A0 and the per-object update through the
// function pointer at arg0+0x4C. State 1: advance the angle by the speed, wrap it
// at +/-pi, scale a func_0011E2A8 lookup of it by 0x2E8 into 0x70003A20, rotate the
// orientation, apply it to both camera roots (func_001026D0), offset the secondary
// root by 4*func_0011E398 of that value, decay the 0x2E8 timer by 0.001 (-> state 4
// below zero), and on the arg0+0x36 retrigger reset like state 4 (timer only);
// close with func_001B17A0 + the arg0+0x4C call. State 3: func_001AFC10(arg0).
extern int func_001B0FD0(char *rec);
extern void func_001C6380(void *p);
extern void copy_qw4(void *dst, void *src);
extern void func_001028D0(void *a, void *b, void *c);
extern void func_001029C0(void *a);
extern void func_00102BB0(void *a, void *b, float t);
extern void func_001026A0(void *a, void *b, void *c);
extern float func_0011E2A8(float x);
extern void func_00102A60(void *a, void *b, float t);
extern void func_001026D0(void *a, void *b, void *c);
extern float func_0011E398(float x);
extern void func_001B17A0(void *p);
extern void func_001AFC10(char *rec);
extern char **D_00275B40;
extern char D_00810350[];
extern char D_700036A0[];
extern volatile float D_700038A0[4];
extern volatile float D_700038A4[4];
extern volatile float D_700038A8[4];
extern volatile float D_700038AC[4];
extern volatile float D_700038B0[4];
extern volatile float D_700038B8[4];
extern float D_70003A20[4];

/* arg0 + 0x1F0: the record's local rig, an array of 4x4 float matrices
 * (the original indexes it as matrices, which keeps the base address
 * unfolded); [0]/[1] are the snapshots of the two camera roots, and
 * row 3 of [3] holds the spin speed, angle and timer (0x2E0/0x2E4/0x2E8). */
#define RIG(r) ((float (*)[4][4])((r) + 0x1F0))

void func_00156F30(char *arg0) {
    float *angle;
    float *timer;
    float *speed;

    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        if (func_001B0FD0(arg0) == 0) {
            *(char *)(arg0 + 0) = 1;
            func_001C6380(arg0);
            copy_qw4(RIG(arg0)[0], D_00275B40[0] + 0x90);
            copy_qw4(RIG(arg0)[1], D_00275B40[1] + 0x90);
            *(unsigned char *)(arg0 + 4) = 4;
        }
        break;
    case 4:
        if (*(short *)(arg0 + 0x36) != 0) {
            *(unsigned char *)(arg0 + 4) = 1;
            *(float *)(arg0 + 0x2E8) = 0.2f;
            *(float *)(arg0 + 0x2E4) = 0.0f;
            *(short *)(arg0 + 0x36) = 0;
            func_001028D0(D_700038A0, arg0 + 0xB0, D_00810350);
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0, -*(float *)(arg0 + 0xC4));
            func_001026A0(D_700038A0, D_700036A0, D_700038A0);
            if (D_700038A0[0] < 0.0f) {
                *(float *)(arg0 + 0x2E0) = 0.08f;
            } else {
                *(float *)(arg0 + 0x2E0) = -0.08f;
            }
        }
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        break;
    case 1:
        speed = &RIG(arg0)[3][3][0];
        angle = &RIG(arg0)[3][3][1];
        *(float *)(arg0 + 0x2E4) += *(float *)(arg0 + 0x2E0);
        if (*(float *)(arg0 + 0x2E4) > 3.1415927f) {
            *(int *)angle = 0xC0490FDB;
        } else if (*(float *)(arg0 + 0x2E4) < -3.1415927f) {
            *(int *)angle = 0x40490FDB;
        }
        D_70003A20[0] = *(float *)(arg0 + 0x2E8) * func_0011E2A8(*angle);
        timer = &RIG(arg0)[3][3][2];
        func_001029C0(D_700036A0);
        func_00102A60(D_700036A0, D_700036A0, D_70003A20[0]);
        func_001C6380(arg0);
        func_001026D0(D_00275B40[0] + 0x90, D_00275B40[0] + 0x90, D_700036A0);
        func_001026D0(D_00275B40[1] + 0x90, D_00275B40[1] + 0x90, D_700036A0);
        D_700038AC[0] = 0.0f;
        D_700038A8[0] = 0.0f;
        D_700038A4[0] = 0.0f;
        D_700038A0[0] = 4.0f * func_0011E398(D_70003A20[0]);
        func_001026A0(D_700038B0, D_00275B40[1] + 0x90, D_700038A0);
        *(float *)(D_00275B40[1] + 0xC0) = *(float *)(D_00275B40[1] + 0xC0) - D_700038B0[0];
        *(float *)(D_00275B40[1] + 0xC8) = *(float *)(D_00275B40[1] + 0xC8) - D_700038B8[0];
        *timer = *timer - 0.001f;
        if (*timer < 0.0f) {
            *(unsigned char *)(arg0 + 4) = 4;
        }
        if (*(short *)(arg0 + 0x36) != 0) {
            *(unsigned char *)(arg0 + 4) = 1;
            *timer = 0.2f;
            *(short *)(arg0 + 0x36) = 0;
            func_001028D0(D_700038A0, arg0 + 0xB0, D_00810350);
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0, -*(float *)(arg0 + 0xC4));
            func_001026A0(D_700038A0, D_700036A0, D_700038A0);
            if (D_700038A0[0] < 0.0f) {
                *speed = 0.08f;
            } else {
                *speed = -0.08f;
            }
        }
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        break;
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
