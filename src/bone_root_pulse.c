// NEARMISS bone_root_pulse  (vram 0xroot_pulse, 0x94C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.25% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 98.27% via mwcc 2.3.3 (-O4,p -sdatathreshold 8); logic and structure fully recovered (593 instructions, 28 residual rows). Residuals are five distinct compiler artifacts, none source-addressable: (1) DELAY-SLOT DUPLICATE-FILL POLICY, and it is ASYMMETRIC — at three sites (the e[5]==2 dispatch beq...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// SEMANTICS: per-frame brain for the "bone root" enemy (entity arg0, tail block at
// arg0+0x1F0). arg0+4 is the behaviour, arg0+5 the state, arg0+6 a sub-step;
// arg0+0x28 is a frame counter, arg0+0x2A the render/scale value pushed to the
// three shorts at *(arg0+0x124)+0x88/0x8A/0x8C, arg0+0x34 the hit-point pool,
// arg0+0x36 the incoming-damage word (low 12 bits = amount, bits 0x8000/0x5000/
// 0x4000 = damage class flags), arg0+0x4C the draw virtual, arg0+0xB0 the position
// and arg0+0xC4 the yaw. arg0+0x118/0x11C/0x124/0x128 are attached sub-objects.
// tail+0x04 is the desired yaw, tail+0x1C a hit-reaction cooldown.
// D_008102B0 is the player; 0x700038A0/0x700038B0 are scratchpad quaternions and
// 0x70003B68 is the free-running frame counter used as a cheap phase gate.
//
// behaviour 0  SPAWN: bind the model (func_001B1020(e,3,-1,0)), install the update
//              tables D_00275650/D_00275658, 0x50 hit points, scale 0x1000.
// behaviour 1  ALIVE, only while func_001B2140() says the world is running:
//   state 0  face away from the current yaw, then fall through into state 1.
//   state 1  chase: func_001BE5F0 steers toward the player. On success the counter
//            ticks and, once past 0x3C frames (or every 64th frame before that),
//            re-aims at the player's XZ with a random jitter of +/-10deg (or
//            +/-15deg on the periodic re-aim) and advances the state. On failure it
//            re-aims fully randomly (+/-90deg) every 128th frame and resets the
//            counter. Either way the sub-object's yaw at +0x74 eases toward
//            tail+0x04 at 2deg/frame (0.4deg/frame while wandering).
//   state 2  keep easing at 2deg/frame; once the yaw has fully arrived, snap the
//            scratch quaternion to the sub-object pose and test it with
//            func_0019A570 - blocked goes back to state 1, clear enters the attack
//            (state 3, sub-step 0).
//   state 3  attack script over arg0+0x2A (the charge/telegraph scale):
//            0 = fire sound 0x43C and reset scale to 0x1000; 1 = wind up +0x20 per
//            frame to 0x2000 then arm a 0x14-frame hold; 2 = hold; 3 = release,
//            -0x100 per frame down to 0x1200; 4 = spawn the strike effect
//            0x8000005A between the two sub-objects and fire sound 0x43B;
//            5 = recover -0x2A per frame to 0x1000, then either re-acquire the
//            player (state 2) or drop back to state 0.
//   Common tail: func_001C6380 rebuilds the transform, the hit-reaction cooldown
//   decays, and any pending damage in +0x36 is applied - a non-stun hit of class
//   0x5000 arms a 0x3C-frame cooldown and (class 0x4000) fires reaction effect
//   0x80000027, the low 12 bits come off the hit-point pool, and reaching 0 flips
//   the entity to behaviour 2 with the death sounds 0x15D and 0x440. Finally the
//   scale is broadcast to the three shorts, func_001B17A0 + the +0x4C virtual draw
//   run, and func_001F4A00 posts the fixed pose 0.323/0.742 with extent 0x20/0x70/
//   0x80/0x80.
// behaviour 2  DYING: state 0 pitches the sub-object at +0x70 up 1deg/frame to a
//              50deg limit, state 1 waits on death effect 0x8000001E (then state 2,
//              or behaviour 3 if the effect could not be spawned), state 2 idles.
//              The scale bleeds off 0x40/frame down to 0x1000 and is broadcast.
// behaviour 3  DESPAWN: release the model slot at +0x9A and free the entity.

extern int func_00102948(char *dst, char *src);
extern int func_001026A0(char *out, char *a, char *b);
extern int func_00122BB8(void);
extern int func_0019A570(char *a, char *b, int c, int d);
extern void func_001AFC10(unsigned char *e);
extern void func_001B1020(unsigned char *e, int t, int a, int b);
extern void func_001B1190(unsigned char id);
extern float func_001B1240(unsigned char *p, float x, float z);
extern float func_001B12B0(float cur, float dst, float rate);
extern float func_001B1470(float ang);
extern void func_001B17A0(unsigned char *e);
extern int func_001B2140(unsigned char *e);
extern int func_001BE5F0(unsigned char *player, unsigned char *e, unsigned char *tail);
extern void func_001C6380(unsigned char *e);
extern char *func_001EFE00(int id, unsigned char *e);
extern void func_001EFFD0(int id, char *a, char *b, int n, float f);
extern void func_001F4A00(char *a, char *b);
extern int func_001FBD50(unsigned char *e, int id, int flags, float f);

extern int D_00275650;
extern int D_00275658;
extern int D_0028A50C[];
extern unsigned char D_008102B0[];
extern char D_700038A0[];
extern char D_700038B0[];

void bone_root_pulse(unsigned char *e) {
    unsigned char *tail = e + 0x1F0;
    unsigned char *player = D_008102B0;
    unsigned char *p;
    float *ang;
    float a;
    short t;
    short f;

    switch (e[4]) {
    case 0:
        func_001B1020(e, 3, -1, 0);
        *(int *)(e + 0x58) = D_0028A50C[0];
        *(short *)(e + 0x52) = 1;
        e[0x5D] = 0x81;
        e[0] = 1;
        *(short *)(e + 0x34) = 0x50;
        *(void **)(e + 0x30) = &D_00275650;
        *(void **)(tail + 0x18) = &D_00275658;
        *(short *)(e + 0x2A) = 0x1000;
        *(int *)(tail + 0x1C) = 0;
        break;
    case 1:
        if (func_001B2140(e) != 0) {
            switch (e[5]) {
            case 0:
                *(short *)(e + 0x28) = 0;
                e[5] = e[5] + 1;
                *(float *)(tail + 4) = func_001B1470(-*(float *)(e + 0xC4));
                /* fallthrough */
            case 1:
                if (func_001BE5F0(player, e, tail) != 0) {
                    *(short *)(e + 0x28) = *(short *)(e + 0x28) + 1;
                    if (*(short *)(e + 0x28) > 0x3C) {
                        e[5] = e[5] + 1;
                        a = func_001B1240(e + 0xB0, *(float *)(player + 0xA0),
                                          *(float *)(player + 0xA8));
                        *(float *)(tail + 4) = func_001B1470(
                            (a + (-0.174532935f + (0.34906587f * (4.65661287e-10f * (float)func_00122BB8()))))
                            - *(float *)(e + 0xC4));
                    } else if ((*(int *)0x70003B68 & 0x3F) == 0) {
                        a = func_001B1240(e + 0xB0, *(float *)(player + 0xA0),
                                          *(float *)(player + 0xA8));
                        *(float *)(tail + 4) = func_001B1470(
                            (a + (-0.261799395f + (0.52359879f * (4.65661287e-10f * (float)func_00122BB8()))))
                            - *(float *)(e + 0xC4));
                    }
                    *(float *)(*(char **)(e + 0x118) + 0x74) =
                        func_001B12B0(*(float *)(tail + 4),
                                      *(float *)(*(char **)(e + 0x118) + 0x74), 0.0349065848f);
                } else {
                    if ((*(int *)0x70003B68 & 0x7F) == 0) {
                        *(float *)(tail + 4) = func_001B1470(
                            (-1.57079637f + (3.14159274f * (4.65661287e-10f * (float)func_00122BB8())))
                            - *(float *)(e + 0xC4));
                    }
                    *(float *)(*(char **)(e + 0x118) + 0x74) =
                        func_001B12B0(*(float *)(tail + 4),
                                      *(float *)(*(char **)(e + 0x118) + 0x74), 0.00698131742f);
                    *(short *)(e + 0x28) = 0;
                }
                break;
            case 2:
                *(float *)(*(char **)(e + 0x118) + 0x74) =
                    func_001B12B0(*(float *)(tail + 4),
                                  *(float *)(*(char **)(e + 0x118) + 0x74), 0.0349065848f);
                p = *(unsigned char **)(e + 0x118);
                if (*(float *)(p + 0x74) == *(float *)(tail + 4)) {
                    func_00102948(D_700038A0, (char *)p + 0xC0);
                    *(float *)0x700038B0 = 0.0f;
                    *(float *)0x700038B4 = 0.0f;
                    *(float *)0x700038B8 = 20.0f;
                    *(float *)0x700038BC = 1.0f;
                    func_001026A0(D_700038B0, *(char **)(e + 0x118) + 0x90, D_700038B0);
                    if (func_0019A570(D_700038A0, D_700038B0, 6, 0) != 0) {
                        *(short *)(e + 0x28) = 0;
                        e[5] = 1;
                    } else {
                        e[5] = e[5] + 1;
                        e[6] = 0;
                    }
                }
                break;
            case 3:
                switch (e[6]) {
                case 0:
                    func_001FBD50(e, 0x43C, 0, 300.0f);
                    *(short *)(e + 0x2A) = 0x1000;
                    e[6] = e[6] + 1;
                    /* fallthrough */
                case 1:
                    *(short *)(e + 0x2A) = *(short *)(e + 0x2A) + 0x20;
                    if (*(short *)(e + 0x2A) > 0x2000) {
                        *(short *)(e + 0x2A) = 0x2000;
                        e[6] = e[6] + 1;
                        *(short *)(e + 0x28) = 0x14;
                    }
                    break;
                case 2:
                    t = *(short *)(e + 0x28) - 1;
                    *(short *)(e + 0x28) = t;
                    if ((long long)t == 0) {
                        e[6] = e[6] + 1;
                    }
                    break;
                case 3:
                    *(short *)(e + 0x2A) = *(short *)(e + 0x2A) - 0x100;
                    if (*(short *)(e + 0x2A) < 0x1200) {
                        e[6] = e[6] + 1;
                    }
                    break;
                case 4:
                    *(float *)0x700038A0 = 0.0f;
                    *(float *)0x700038A4 = 1.75f;
                    *(float *)0x700038A8 = 7.90999985f;
                    *(float *)0x700038AC = 1.0f;
                    func_001026A0(D_700038A0, *(char **)(e + 0x11C) + 0x90, D_700038A0);
                    *(float *)0x700038B0 = 0.0f;
                    *(float *)0x700038B4 = 0.0f;
                    *(float *)0x700038B8 = 1.0f;
                    *(float *)0x700038BC = 0.0f;
                    func_001026A0(D_700038B0, *(char **)(e + 0x118) + 0x90, D_700038B0);
                    func_001EFFD0(0x8000005A, D_700038A0, D_700038B0, 0x14, 2.0f);
                    func_001FBD50(e, 0x43B, 0, 300.0f);
                    e[6] = e[6] + 1;
                    *(short *)(e + 0x2A) = *(short *)(e + 0x2A) - 0x100;
                    break;
                case 5:
                    *(short *)(e + 0x2A) = *(short *)(e + 0x2A) - 0x2A;
                    if (*(short *)(e + 0x2A) < 0x1000) {
                        *(short *)(e + 0x2A) = 0x1000;
                        if (func_001BE5F0(player, e, tail) != 0) {
                            e[5] = 2;
                            a = func_001B1240(e + 0xB0, *(float *)(player + 0xA0),
                                              *(float *)(player + 0xA8));
                            *(float *)(tail + 4) = func_001B1470(
                                (a + (-0.174532935f + (0.34906587f * (4.65661287e-10f * (float)func_00122BB8()))))
                                - *(float *)(e + 0xC4));
                        } else {
                            e[5] = 0;
                        }
                    }
                    break;
                }
                break;
            }
            func_001C6380(e);
            if (*(int *)(tail + 0x1C) != 0) {
                *(int *)(tail + 0x1C) = *(int *)(tail + 0x1C) - 1;
            }
            f = *(short *)(e + 0x36);
            if (f != 0) {
                if (((long long)f & 0x8000) == 0 && ((long long)f & 0x5000) != 0) {
                    if (*(int *)(tail + 0x1C) == 0) {
                        *(int *)(tail + 0x1C) = 0x3C;
                        if ((*(short *)(e + 0x36) & 0x4000) != 0) {
                            func_001EFE00(0x80000027, e);
                        }
                    } else {
                        *(short *)(e + 0x36) = 0;
                    }
                }
                *(short *)(e + 0x34) =
                    *(short *)(e + 0x34) - (*(short *)(e + 0x36) & 0xFFF);
                if (*(short *)(e + 0x34) <= 0) {
                    e[4] = 2;
                    e[5] = 0;
                    e[0] = 2;
                    func_001FBD50(e, 0x15D, 0, 300.0f);
                    func_001FBD50(e, 0x440, 0, 300.0f);
                } else {
                    e[0] = 1;
                    *(short *)(e + 0x36) = 0;
                }
            }
            t = *(short *)(e + 0x2A);
            *(short *)(*(char **)(e + 0x124) + 0x8C) = t;
            *(short *)(*(char **)(e + 0x124) + 0x8A) = t;
            *(short *)(*(char **)(e + 0x124) + 0x88) = t;
            func_001B17A0(e);
            (*(void (**)(unsigned char *))(e + 0x4C))(e);
            *(float *)0x700038A0 = 0.0f;
            *(float *)0x700038A4 = 0.323000014f;
            *(float *)0x700038A8 = 0.741999984f;
            *(float *)0x700038AC = 1.0f;
            func_001026A0(D_700038A0, *(char **)(e + 0x128) + 0x90, D_700038A0);
            *(int *)0x700038B0 = 0x20;
            *(int *)0x700038B4 = 0x70;
            *(int *)0x700038B8 = 0x80;
            *(int *)0x700038BC = 0x80;
            func_001F4A00(D_700038A0, D_700038B0);
        }
        break;
    case 2:
        switch (e[5]) {
        case 0:
            p = *(unsigned char **)(e + 0x11C);
            *(float *)(p + 0x70) = *(float *)(p + 0x70) + 0.0174532924f;
            ang = (float *)(*(unsigned char **)(e + 0x11C) + 0x70);
            if (!(*ang <= 0.87266463f)) {
                *ang = 0.87266463f;
                e[5] = e[5] + 1;
            }
            break;
        case 1:
            if (func_001EFE00(0x8000001E, e) != 0) {
                e[5] = 2;
            } else {
                e[4] = 3;
            }
            break;
        case 2:
            break;
        }
        *(short *)(e + 0x2A) = *(short *)(e + 0x2A) - 0x40;
        if (*(short *)(e + 0x2A) < 0x1000) {
            *(short *)(e + 0x2A) = 0x1000;
        }
        t = *(short *)(e + 0x2A);
        *(short *)(*(char **)(e + 0x124) + 0x8C) = t;
        *(short *)(*(char **)(e + 0x124) + 0x8A) = t;
        *(short *)(*(char **)(e + 0x124) + 0x88) = t;
        func_001C6380(e);
        (*(void (**)(unsigned char *))(e + 0x4C))(e);
        break;
    case 3:
        func_001B1190(e[0x9A]);
        func_001AFC10(e);
        break;
    }
}
