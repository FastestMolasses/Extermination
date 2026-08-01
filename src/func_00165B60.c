// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// SEMANTICS: entity think-state dispatcher on the state byte e[6].
//   case 0: if the world/camera mode (D_00810700[0]) is 0xD and the entity
//           position (e+0xB0..0xB8) is inside a fixed box, snap to state 0xA
//           and stamp e[0x1F0]=0x15; otherwise advance the state and play the
//           fallback anim 0xE3 or 0xE4 depending on e[0x1F0].
//   case 1: wait until input bit 0x8000 clears, then advance.
//   case 2: the big body. On the advance signal (bit 0x1000) rebuild the
//           orientation matrix at e+0xD0, transform the fixed offset staged in
//           the scratchpad at 0x700038A0 through it, re-anchor the entity onto
//           the active camera record (*(D_00275B40+4) + 0xC0), then start clip
//           D_002754D0[0] and hand off to outer state 0xC. Otherwise, for the
//           0x16 variant, drive a two-step ledge/drop interpolation on e[7]
//           (measuring the drop with func_00199FA0/func_0011DF78 into
//           0x70003A20 and easing e+0xB4 by e+0x2E4 per frame). Independently
//           step the footstep/impact sub-state machine (e+0x28 for the 0x16
//           variant, e[7] otherwise) off the clip frame counter e+0x3C,
//           firing sound cue 0x107 at 300.0 on the last step.
//   case 10: bump the state and kick off clip 0x70.
//   case 11: on the advance signal seed the fixed pose (1021.9/187.2/836.3)
//            and play clip 0xFE.
//   case 12: on the advance signal restart clip D_002754D0[0] and hand off to
//            outer state 0xC.
//   Tail: if the world/camera mode is 2, run func_00176DC0.
//
//   NOTE on the declarations: the original TU was built with a small-data
//   threshold, so D_00275B40 (a 4-byte pointer) is reached via
//   %gp_rel($gp) while every other global here stays absolute %hi/%lo. That is
//   reproduced by compiling with -sdatathreshold 4 and declaring the far
//   globals as arrays larger than the threshold; with -sdatathreshold 0 the
//   D_00275B40 loads lower to lui/lw and the function stops at 99.54%.

extern void *build_trs_matrix(void *mtx, void *pos, void *rot, void *scale);
extern void func_001026A0(void *dst, void *src, void *xf);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_00102918(void *dst, void *a, void *b);
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *a, void *b);
extern int func_001031E0(void *dst, void *src, unsigned char n);
extern float func_0011DF78(float);
extern int func_001749A0(unsigned char *e, int clip, int flags, float blend);
extern void func_00176DC0(unsigned char *e);
extern void func_0017FC80(unsigned char *e, float f);
extern int func_00180300(unsigned char *e, void *m, unsigned char b);
extern void func_00182A70(void *e);
extern int func_00199FA0(char *arg0, char *arg1);
extern float func_001B1470(float a);
extern void func_001FBD50(unsigned char *e, int id, int b, float f);

extern short D_002754D0[16];
extern int *D_00275B40;
extern unsigned char D_00810700[16];
extern int D_700038A0[16];
extern int D_700038B0[16];

void func_00165B60(unsigned char *e) {
    unsigned char st;
    unsigned char v1;
    float sp20[4];
    float sp30[4];

    st = e[6];
    switch (st) {
    case 0:
        if (D_00810700[0] == 0xD && *(float *)(e + 0xB0) >= 1010.0f && *(float *)(e + 0xB0) <= 1030.0f
            && *(float *)(e + 0xB4) >= 170.0f && *(float *)(e + 0xB4) <= 180.0f
            && *(float *)(e + 0xB8) >= 830.0f && *(float *)(e + 0xB8) <= 850.0f) {
            e[6] = 0xA;
            e[7] = 0;
            v1 = 0x15;
            e[0x1F0] = v1;
        } else {
            e[6] = *(volatile unsigned char *)(e + 6) + 1;
            e[7] = 0;
            if (e[0x1F0] == 0x15) {
                func_001749A0(e, 0xE3, 0, 8.0f);
            } else {
                func_001749A0(e, 0xE4, 0, 8.0f);
                *(short *)(e + 0x28) = 0;
            }
        }
        break;
    case 1:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            e[6] = st + 1;
        }
        break;
    case 2:
        if (*(int *)(e + 0x200) & 0x1000) {
            if (e[0x1F0] == 0x16) {
                *(float *)(e + 0xC4) = func_001B1470(3.1415927f + *(float *)(e + 0xC4));
            }
            func_001029C0(e + 0xD0);
            func_00102C58(e + 0xD0, e + 0xD0, e + 0xC0);
            *(int *)0x700038A0 = 0;
            *(int *)0x700038A4 = 0xC1266666;
            *(int *)0x700038A8 = 0x40C9999A;
            *(int *)0x700038AC = 0;
            func_001026A0(D_700038B0, e + 0xD0, D_700038A0);
            func_001028B8(e + 0xB0, (void *)(*(int *)((char *)D_00275B40 + 4) + 0xC0), D_700038B0);
            *(int *)(e + 0xBC) = 0x3F800000;
            func_00102918(e + 0xD0, e + 0xD0, e + 0xB0);
            if (e[0x1F0] == 0x16) {
                *(float *)(e + 0xB4) = *(float *)(e + 0x294);
            }
            e[0x2F1] = 0;
            func_001749A0(e, D_002754D0[0], 0, 0.0f);
            func_0017FC80(e, 16.0f);
            e[5] = 0xC;
            e[6] = 0;
            e[0x1F0] = 0x17;
        } else if (e[0x1F0] == 0x16) {
            switch (e[7]) {
            case 0:
                if (*(float *)(e + 0x3C) <= 25.0f) {
                    func_001031E0(D_700038A0, (void *)(*(int *)((char *)D_00275B40 + 4) + 0xC0), st);
                    *(float *)(e + 0xC4) = func_001B1470(3.1415927f + *(float *)(e + 0xC4));
                    build_trs_matrix(e + 0xD0, e + 0xB0, e + 0xC0, e + 0x60);
                    if (func_00180300(e, D_700038A0, 0) == 0 && func_00199FA0((char *)sp20, (char *)sp30) != 0) {
                        *(float *)0x70003A20 = func_0011DF78(sp30[1] - sp20[1]);
                        *(float *)0x70003A20 = *(float *)0x70003A20 - 16.0f;
                        do {
                            *(float *)0x70003A20 = *(float *)0x70003A20 - 3.0f;
                        } while (!(*(float *)0x70003A20 < 3.0f));
                        e[7] = e[7] + 1;
                        *(float *)(e + 0x294) = *(float *)(e + 0xB4) - (16.0f + *(float *)0x70003A20);
                        *(float *)(e + 0x2E4) = *(float *)0x70003A20 / (*(float *)(e + 0x3C) - 1.0f);
                    }
                    *(float *)(e + 0xC4) = func_001B1470(3.1415927f + *(float *)(e + 0xC4));
                }
                break;
            case 1:
                *(float *)(e + 0xB4) -= *(float *)(e + 0x2E4);
                break;
            }
        }
        if (e[0x1F0] == 0x16) {
            switch (*(short *)(e + 0x28)) {
            case 0:
                if (*(float *)(e + 0x3C) <= 63.0f) {
                    *(short *)(e + 0x28) = *(short *)(e + 0x28) + 1;
                    func_00182A70(e);
                }
                break;
            case 1:
                if (*(float *)(e + 0x3C) <= 52.0f) {
                    *(short *)(e + 0x28) = *(short *)(e + 0x28) + 1;
                    func_00182A70(e);
                }
                break;
            case 2:
                if (*(float *)(e + 0x3C) <= 20.0f) {
                    *(short *)(e + 0x28) = *(short *)(e + 0x28) + 1;
                    func_00182A70(e);
                }
                break;
            case 3:
                if (*(float *)(e + 0x3C) <= 2.0f) {
                    *(short *)(e + 0x28) = *(short *)(e + 0x28) + 1;
                    func_00182A70(e);
                    func_001FBD50(e, 0x107, 0, 300.0f);
                }
                break;
            }
        } else {
            switch (e[7]) {
            case 0:
                if (*(float *)(e + 0x3C) <= 24.0f) {
                    e[7] = e[7] + 1;
                    func_00182A70(e);
                }
                break;
            case 1:
                if (*(float *)(e + 0x3C) <= 2.0f) {
                    e[7] = e[7] + 1;
                    func_00182A70(e);
                    func_001FBD50(e, 0x107, 0, 300.0f);
                }
                break;
            case 2:
                break;
            }
        }
        break;
    case 10:
        e[6] = st + 1;
        func_001749A0(e, 0x70, 0, 1.0f);
        break;
    case 11:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[6] = st + 1;
            *(float *)(e + 0xB0) = 1021.9f;
            *(float *)(e + 0xB4) = 187.2f;
            *(float *)(e + 0xB8) = 836.3f;
            func_001749A0(e, 0xFE, 0, 0.0f);
        }
        break;
    case 12:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[0x2F1] = 0;
            func_001749A0(e, D_002754D0[0], 0, 0.0f);
            func_0017FC80(e, 16.0f);
            e[5] = 0xC;
            e[6] = 0;
            v1 = 0x17;
            e[0x1F0] = v1;
        }
        break;
    }
    if (D_00810700[0] == 2) {
        func_00176DC0(e);
    }
}
