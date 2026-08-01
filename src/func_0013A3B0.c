// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS: enemy "stalker" AI update. `e` = entity, `st` = its AI state block.
// e[6] is the sub-state, dispatched through a 10-entry jump table (jtbl_0026D1E0;
// slot 8 is the switch exit, i.e. an unused state). Two blend rates are latched on
// entry and consumed by the shared tail: `yawrate` (entity yaw e+0xC4, deg/frame)
// and `turnrate` (steer angle st+0x50, deg/frame). `reset` (states 7 and 9) rewinds
// the whole brain to state 0 in the tail.
//   0  spawn: clear steer/speed, face +90 deg, play clip 6, fall through to 1.
//   1  approach: decay speed st+0x44; cancel the turn once close to the player or
//      when st[0x81]&4 (blocked). When fully stopped and e's flag word e+0x2C (bit
//      15 masked out) reads 7, arm the 300-frame timer, fire event 0x818 and clip 8.
//      Always re-aims st+0x58 at the player (func_001B1240).
//   2  circle-strafe: count st+0x20 down; while the player stays inside the
//      1.4835 rad cone (func_001B1560) run the strafe (speed clamp 0.8, lateral
//      gain 0.4/0.65, yaw from func_001B1270) and test the lunge volume
//      (func_001B34F0 over a 4 x 9 box). On a hit, and if the grab slot D_008102B0
//      is free, latch the grab (func_0021BF90), snap the entity 3.7 units behind
//      the player along D_00810374 and advance with clip 9 + event 0x818.
//      Otherwise (timer expired / cone lost / st[0x81] set) go to state 9, clip 0xE.
//   3  grab wind-up: hold the entity 8 units above D_00810364 until st+0x70&0x1000,
//      then state 4 with the 240/60 frame timers and clip 0xB.
//   4  grab hold: keep the entity pinned 8 up / 3.7 behind; tick the 240 and 60
//      frame timers and every 60 frames fire event 0x819 and drain the player gauge
//      D_008104D0 toward a floor of 10/12/15 (variant e[0xD]&0x80 and D_0081070A).
//      On the floor, or when the main timer expires, release to state 6 / advance
//      to state 5 with clip 0xC.
//   5  throw: once st+0x20 is idle and the entity is within 17 units, fire event
//      0x81A, spawn the impact effect at the offset (0, -5.0768, 3.7116), set the
//      recovery target D_008104DC (40/48/60) and release the grab (func_0021C040).
//   6  recover: reset the pose and play clip 0xD.
//   7  cooldown: hold speed at 0.4; on st+0x70&0x1000 request the full reset.
//   9  stagger/retreat: integrate the (negative) speed along st+0x60, keep pushing
//      the entity out of the world (func_0019AD00), request the reset on 0x1000.
// D_00810360/364/368 = player world position. D_008102B0 = grab-slot state block.
// D_00810374 = latched grab bearing. D_008106BD = external cancel request.
// D_0081070A = "hard mode" flag. 0x700036A0 = scratchpad matrix, 0x700038A0/B0 = vec4 temps.

extern volatile unsigned char D_008102B0;
extern float D_00810360;
extern float D_00810364;
extern float D_00810368;
extern int D_00810370;
extern float D_00810374;
extern float D_008104D0;
extern volatile float D_008104D4;
extern float D_008104DC;
extern unsigned char D_008106BD;
extern unsigned char D_0081070A;
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];

extern void anim_clip_init(void *e, int id, float a, float b);
extern void func_001026A0(void *dst, void *m, void *v);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001029C0(void *m);
extern void func_00102BB0(void *dst, void *m, float ang);
extern void func_00102C58(void *dst, void *m, void *v);
extern float func_0011DE90(float a);
extern float func_0011E2A8(float a);
extern int func_00122BB8(void);
extern void func_0019AD00(void *e, void *v, unsigned int id);
extern float func_001B1240(void *pos, float px, float pz);
extern float func_001B1270(void *pos, float px, float pz);
extern float func_001B12B0(float target, float cur, float step);
extern float func_001B1470(float a);
extern int func_001B1560(void *e, float r, void *pos);
extern void func_001B2B10(void *e, void *a, void *b);
extern int func_001B34F0(void *a, void *b, void *c);
extern void func_001EFD90(unsigned int id, void *pos, void *out);
extern void func_001EFEB0(unsigned int id, void *m);
extern void func_001FBD50(void *e, int id, int flag, float a);
extern int func_0021BED0(void *s);
extern void func_0021BF90(void *s, void *e);
extern void func_0021C040(void *s, void *e);

void func_0013A3B0(unsigned char *e, unsigned char *st) {
    float yawrate;
    float turnrate;
    float lim;
    float t0;
    float z;
    float zf;
    float hz;
    float hy;
    float hx;
    int zi;
    int sw;
    int reset;

    sw = *(volatile unsigned char *)(e + 6);
    yawrate = 1.5f;
    turnrate = 1.0f;
    reset = 0;
    switch (sw) {
    case 0:
        e[6] = e[6] + 1;
        *(float *)(st + 0x50) = 0.0f;
        *(float *)(st + 0x4C) = 0.4f;
        st[0x84] = 1;
        *(float *)(st + 0x5C) = 1.5707964f;
        anim_clip_init(e, 6, 5.0f, 0.0f);
        /* fallthrough */
    case 1:
        if (*(float *)(st + 0x44) <= 0.0f) {
            *(float *)(st + 0x48) = 0.0f;
            *(float *)(st + 0x44) = 0.0f;
        } else {
            *(float *)(st + 0x48) = -0.005f;
        }
        if (*(float *)(st + 0x5C) != 0.0f) {
            if (*(char *)(st + 0x81) & 4) {
                *(float *)(st + 0x5C) = 0.0f;
                *(float *)(st + 0x50) = 0.0f;
            } else if (!(*(float *)(e + 0xB4) < 15.0f + D_00810364)) {
                *(float *)(st + 0x5C) = 0.0f;
            }
        } else {
            turnrate = 2.5f;
        }
        if (*(float *)(st + 0x50) == (z = 0.0f) &&
            *(float *)(st + 0x5C) == (z = 0.0f) &&
            *(float *)(st + 0x44) == (z = 0.0f) &&
            (*(short *)(e + 0x2C) & 0xFFFF7FFF) == 7) {
            e[6] = e[6] + 1;
            *(unsigned short *)(st + 0x20) = 300;
            *(float *)(st + 0x48) = 0.0f;
            *(float *)(st + 0x44) = 0.0f;
            func_001FBD50(e, 0x818, 0, 300.0f);
            anim_clip_init(e, 8, 5.0f, 0.0f);
        }
        *(float *)(st + 0x58) = func_001B1240(e + 0xB0, D_00810360, D_00810368);
        break;
    case 2:
        if (*(char *)(st + 0x81) == 0) {
            int tm = *(unsigned short *)(st + 0x20) - 1;
            *(unsigned short *)(st + 0x20) = tm;
            if ((tm & 0xFFFF) != 0 &&
                func_001B1560(e, 1.4835298f, &D_00810360) != 0) {
                goto state2_track;
            }
        }
        zi = 0;
        zf = (float)zi;
        e[6] = 9;
        st[0x82] = 1;
        *(float *)(st + 0x4C) = 0.4f;
        *(float *)(st + 0x5C) = 1.5707964f;
        *(float *)(st + 0x60) = *(float *)(e + 0xC4);
        *(float *)(st + 0x40) = -0.005f;
        *(float *)(st + 0x58) = func_001B1470(3.1415927f + *(float *)(e + 0xC4));
        anim_clip_init(e, 0xE, 1.0f, zf);
        break;
    state2_track:
        {
            {
                *(float *)(st + 0x48) = 0.0f;
                if (!(*(float *)(st + 0x44) < 0.8f)) {
                    *(float *)(st + 0x44) = 0.8f;
                } else {
                    *(float *)(st + 0x48) = 0.009f;
                }
                if (*(float *)(st + 0x50) < 0.0f) {
                    *(float *)(st + 0x4C) = 0.65f;
                } else {
                    *(float *)(st + 0x4C) = 0.4f;
                }
                *(float *)(st + 0x5C) =
                    -1.0f * func_001B1270(e + 0xB0, 6.0f + D_00810364,
                                          3.0f + *(float *)(e + 0xB8));
                func_001029C0(&D_700036A0);
                func_00102BB0(&D_700036A0, &D_700036A0, *(float *)(e + 0xC4));
                *(float *)0x700038B0 = 0.0f;
                *(float *)0x700038B4 = 0.0f;
                *(float *)0x700038B8 = 3.5f;
                *(int *)0x700038BC = 0x3F800000;
                func_001026A0(&D_700038B0, &D_700036A0, &D_700038B0);
                func_001028B8(&D_700038B0, &D_700038B0, e + 0xB0);
                *(float *)0x700038A0 = 4.0f;
                *(float *)0x700038A4 = 9.0f;
                *(float *)0x700038A8 = 0.0f;
                *(int *)0x700038AC = 0x3F800000;
                if (func_001B34F0(&D_00810360, &D_700038A0, &D_700038B0) != 0 &&
                    func_0021BED0((void *)&D_008102B0) == 0) {
                    e[6] = e[6] + 1;
                    *(float *)(st + 0x50) = 0.0f;
                    *(float *)(st + 0x5C) = 1.5707964f;
                    *(float *)(st + 0x48) = 0.0f;
                    *(float *)(st + 0x44) = 0.0f;
                    func_0021BF90((void *)&D_008102B0, e);
                    st[0x83] = 1;
                    D_00810374 = func_001B1470(3.1415927f + *(float *)(e + 0xC4));
                    func_001029C0(&D_700036A0);
                    func_00102BB0(&D_700036A0, &D_700036A0, D_00810374);
                    *(float *)0x700038A0 = 0.0f;
                    *(float *)0x700038A4 = 0.0f;
                    *(float *)0x700038A8 = 3.7f;
                    *(int *)0x700038AC = 0x3F800000;
                    func_001026A0(&D_700038A0, &D_700036A0, &D_700038A0);
                    func_001028B8(&D_700038A0, &D_00810360, &D_700038A0);
                    *(float *)(e + 0xB0) = *(float *)0x700038A0;
                    *(float *)(e + 0xB8) = *(float *)0x700038A8;
                    anim_clip_init(e, 9, 1.0f, 0.0f);
                    func_001FBD50(e, 0x818, 0, 300.0f);
                }
            }
        }
        break;
    case 3:
        if (D_008106BD == 1) {
            e[6] = 6;
            st[0x83] = 0;
            D_008106BD = 0;
        } else {
            t0 = 8.0f + D_00810364;
            if (t0 <= *(float *)(e + 0xB4)) {
                *(float *)(e + 0xB4) = t0;
                *(float *)(st + 0x5C) = 0.0f;
                *(float *)(st + 0x50) = 0.0f;
            }
            if (*(int *)(st + 0x70) & 0x1000) {
                e[6] = e[6] + 1;
                *(unsigned short *)(st + 0x20) = 240;
                *(unsigned short *)(st + 0x22) = 60;
                *(float *)(e + 0xB4) = 8.0f + D_00810364;
                *(float *)(st + 0x5C) = 0.0f;
                *(float *)(st + 0x50) = 0.0f;
                anim_clip_init(e, 0xB, 1.0f, 0.0f);
            }
        }
        break;
    case 4:
        if (D_008106BD == 1) {
            e[6] = 6;
            st[0x83] = 0;
            D_008106BD = 0;
        } else {
            int tm;
            int tm2;
            func_001029C0(&D_700036A0);
            func_00102BB0(&D_700036A0, &D_700036A0, D_00810374);
            *(float *)0x700038A0 = 0.0f;
            *(float *)0x700038A4 = 8.0f;
            *(float *)0x700038A8 = 3.7f;
            *(int *)0x700038AC = 0x3F800000;
            func_001026A0(&D_700038A0, &D_700036A0, &D_700038A0);
            func_001028B8(&D_700038A0, &D_00810360, &D_700038A0);
            *(float *)(e + 0xB0) = *(float *)0x700038A0;
            *(float *)(e + 0xB4) = *(float *)0x700038A4;
            *(float *)(e + 0xB8) = *(float *)0x700038A8;
            tm = *(unsigned short *)(st + 0x20);
            if (tm != 0) {
                *(unsigned short *)(st + 0x20) = tm - 1;
                tm2 = *(unsigned short *)(st + 0x22);
                if (tm2 != 0) {
                    *(unsigned short *)(st + 0x22) = tm2 - 1;
                } else {
                    *(unsigned short *)(st + 0x22) = 60;
                    func_001FBD50(e, 0x819, 0, 300.0f);
                    if (e[0xD] & 0x80) {
                        if (D_0081070A != 0) {
                            lim = 15.0f;
                        } else {
                            lim = 12.0f;
                        }
                    } else if (D_0081070A != 0) {
                        lim = 15.0f;
                    } else {
                        lim = 10.0f;
                    }
                    if (D_008104D0 <= lim) {
                        e[6] = 6;
                        st[0x83] = 0;
                        D_008104D4 = D_008104D0;
                        D_008102B0 = D_008102B0 | 2;
                    } else {
                        D_008104D4 = lim;
                        D_008102B0 = D_008102B0 | 2;
                    }
                    func_001029C0(&D_700036A0);
                    func_00102BB0(&D_700036A0, &D_700036A0, D_00810374);
                    *(float *)0x700038A0 = 0.0f;
                    *(float *)0x700038A4 = 3.0f;
                    *(float *)0x700038A8 = 3.0f;
                    *(int *)0x700038AC = 0x3F800000;
                    func_001026A0(&D_700038A0, &D_700036A0, &D_700038A0);
                    func_001028B8(&D_700038A0, &D_00810360, &D_700038A0);
                    func_001EFD90(0x80000006, &D_700038A0, &D_00810370);
                }
            } else {
                e[6] = e[6] + 1;
                anim_clip_init(e, 0xC, 1.0f, 0.0f);
            }
        }
        break;
    case 5:
        if (*(int *)(st + 0x70) & 0x1000) {
            e[6] = sw + 1;
        }
        if (*(unsigned short *)(st + 0x20) == 0 && *(float *)(e + 0x3C) <= 17.0f) {
            *(unsigned short *)(st + 0x20) = 1;
            func_001FBD50(e, 0x81A, 0, 300.0f);
            func_001029C0(&D_700036A0);
            func_00102BB0(&D_700036A0, &D_700036A0, *(float *)(e + 0xC4));
            *(float *)0x700038A0 = 0.0f;
            *(float *)0x700038A4 = -5.0768f;
            *(float *)0x700038A8 = 3.7116f;
            *(int *)0x700038AC = 0x3F800000;
            func_001026A0(&D_700038A0, &D_700036A0, &D_700038A0);
            func_001028B8(&D_700038A0, e + 0xB0, &D_700038A0);
            hx = *(volatile float *)0x700038A0;
            hy = *(volatile float *)0x700038A4;
            hz = *(volatile float *)0x700038A8;
            *(float *)0x700036D0 = hx;
            *(float *)0x700036D4 = hy;
            *(float *)0x700036D8 = hz;
            func_001EFEB0(0x80000029, &D_700036A0);
            if (e[0xD] & 0x80) {
                if (D_0081070A != 0) {
                    lim = 60.0f;
                } else {
                    lim = 40.0f;
                }
            } else if (D_0081070A != 0) {
                lim = 48.0f;
            } else {
                lim = 40.0f;
            }
            D_008104DC = lim;
            D_008102B0 = D_008102B0 | 2;
            st[0x83] = 0;
            func_0021C040((void *)&D_008102B0, e);
            if (D_008106BD == 1) {
                D_008106BD = 0;
            }
        }
        break;
    case 6:
        e[6] = sw + 1;
        *(float *)(st + 0x50) = 0.0f;
        *(float *)(st + 0x5C) = 0.7853982f;
        *(float *)(st + 0x44) = -0.4f;
        *(float *)(st + 0x48) = 0.01f;
        st[0x83] = 0;
        anim_clip_init(e, 0xD, 1.0f, 0.0f);
        break;
    case 7:
        if (*(int *)(st + 0x70) & 0x1000) {
            *(float *)(st + 0x44) = 0.4f;
            *(float *)(st + 0x48) = 0.0f;
            reset = 1;
            *(float *)(st + 0x5C) = 0.0f;
        } else {
            if (!(*(float *)(st + 0x44) <= 0.4f)) {
                *(float *)(st + 0x44) = 0.4f;
                *(float *)(st + 0x48) = 0.0f;
            }
            if (*(float *)(st + 0x44) < 0.0f) {
                *(float *)0x700038A0 = 0.0f;
                *(float *)0x700038A4 = 0.0f;
                *(float *)0x700038A8 = -3.0f;
                *(int *)0x700038AC = 0x3F800000;
                func_001B2B10(e, &D_700038A0, &D_700038A0);
                func_001028B8(&D_700038A0, &D_700038A0, e + 0xB0);
                func_0019AD00(e, &D_700038A0, 0x80000006);
            }
        }
        break;
    case 9:
        if (*(int *)(st + 0x70) & 0x1000) {
            reset = 1;
        }
        yawrate = 2.5f;
        if (*(float *)(st + 0x5C) != 0.0f) {
            if (*(char *)(st + 0x81) & 4) {
                *(float *)(st + 0x5C) = 0.0f;
                *(float *)(st + 0x50) = 0.0f;
            } else if (!(*(float *)(e + 0xB4) < 15.0f + D_00810364)) {
                *(float *)(st + 0x5C) = 0.0f;
            }
        }
        if (*(float *)(st + 0x44) <= 0.0f) {
            *(float *)(st + 0x48) = 0.0f;
            *(float *)(st + 0x44) = 0.0f;
        } else {
            *(float *)(st + 0x48) = *(float *)(st + 0x40);
        }
        *(float *)(st + 0x44) += *(float *)(st + 0x48);
        *(float *)(e + 0xB4) =
            *(float *)(e + 0xB4) +
            *(float *)(st + 0x4C) * func_0011E2A8(*(float *)(st + 0x50));
        *(float *)(e + 0xB0) =
            *(float *)(e + 0xB0) +
            *(float *)(st + 0x44) * func_0011E2A8(*(float *)(st + 0x60));
        *(float *)(e + 0xB8) =
            *(float *)(e + 0xB8) +
            *(float *)(st + 0x44) * func_0011DE90(*(float *)(st + 0x60));
        *(float *)0x700038A0 = 0.0f;
        *(float *)0x700038A4 = *(float *)(st + 0x60);
        *(float *)0x700038A8 = 0.0f;
        *(int *)0x700038AC = 0x3F800000;
        func_001029C0(&D_700036A0);
        func_00102C58(&D_700036A0, &D_700036A0, &D_700038A0);
        *(float *)0x700038A0 = 0.0f;
        *(float *)0x700038A4 = 0.0f;
        *(float *)0x700038A8 = 3.0f;
        *(int *)0x700038AC = 0x3F800000;
        func_001026A0(&D_700038A0, &D_700036A0, &D_700038A0);
        func_001028B8(&D_700038A0, &D_700038A0, e + 0xB0);
        func_0019AD00(e, &D_700038A0, 0x80000006);
        break;
    }
    if (reset != 0) {
        e[5] = 1;
        e[6] = 0;
        *(unsigned short *)(st + 0x20) = 60;
        *(unsigned short *)(st + 0x22) = 0;
        *(short *)(st + 0x2C) = 0;
        *(short *)(st + 0x30) = 0;
        st[0x82] = 0;
        st[0x84] = 0;
        *(float *)(st + 0x5C) = 0.0f;
        *(float *)(st + 0x4C) = 0.4f;
        *(float *)(st + 0x48) = 0.0f;
        *(float *)(st + 0x60) = *(float *)(e + 0xC4);
        *(short *)(st + 0x2E) = ((func_00122BB8() >> 15) & 0x7F) + 0x78;
        t0 = *(float *)(st + 0x44);
        if (t0 < 0.0f) {
            *(float *)(st + 0x44) = 0.0f;
        } else if (!(t0 <= 0.4f)) {
            *(float *)(st + 0x44) = 0.4f;
        }
    }
    *(float *)(st + 0x50) = func_001B12B0(*(float *)(st + 0x5C),
                                          *(float *)(st + 0x50),
                                          (3.1415927f * turnrate) / 180.0f);
    *(float *)(e + 0xC8) = func_001B12B0(0.0f, *(float *)(e + 0xC8), 0.034906585f);
    *(float *)(e + 0xC4) = func_001B12B0(*(float *)(st + 0x58),
                                         *(float *)(e + 0xC4),
                                         (3.1415927f * yawrate) / 180.0f);
}
