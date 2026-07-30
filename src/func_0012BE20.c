// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// SEMANTICS: per-state driver for enemy arg0 with its controller block arg1,
// dispatched on the state byte at arg0+6 (jump table jtbl_0026D080, 8 states).
// func_001C2770(arg0, arg1, 2) is polled once up front and returns a mode code;
// mode 0 additionally runs func_001C3D60(arg0, arg1) as the tail. arg0+5 is the
// behaviour id, arg0+6 the state, arg0+7 a sub-step, arg0+0xB0..0xB8 the position,
// arg0+0xC0/0xC4 pitch/yaw, arg0+0xD0 an orientation quat. arg1+0xD8 is move speed,
// arg1+0xE4 a contact bitmask, arg1+0xF0 a lift rate, arg1+0xF4 an event mask.
// 0x700038A0/0x700038B0 are scratchpad quaternion scratch.
// State 0 (only when mode==0) probes with func_001C25E0; on failure it resets to
// behaviour 2 / state 0, otherwise it backs the position off by half the controller
// velocity, aligns to the surface (func_001C2540) and advances to state 2 if the
// contact mask is exactly 0x100, else state 1. State 1 defers to func_0012D580.
// State 2 plays anim 0xD and advances. State 3 waits for event 0x1000 then plays
// anim 0xE (otherwise ticks func_0012E2C0). State 4 waits for events 0x5000, plays
// anim 0xF, arms a 0x78-frame timer, sets the lift rate to 0.6 and fires sound 0x1AA.
// State 5 lifts: pitch = pi*(75*-liftRate)/180, the rate decays by 0.04 per frame and
// on going negative the state advances with mask 0x400; while pitch is in
// [-10deg, 0) and func_001B13F0 reports the player within 50 units it plays anim 0x10
// and spawns effect 0xA/0xB. State 6 finishes on contact 0x100 (anim 0x11). State 7
// waits for event 0x1000, plays anims 0/1 and flips the yaw by pi.
// States 5-7 drop back to behaviour 1 / state 0 when func_00128640 fails.

extern void func_001026A0(char *a, char *b, char *out);
extern int func_00128640(char *e);
extern void func_001287F0(char *e, char *c, int anim, float blend);
extern void func_00128830(char *e, int st, float a, float b, float c);
extern void func_0012D580(char *e, char *c, int mode);
extern void func_0012E2C0(char *e, char *c);
extern int func_001B13F0(char *a, char *b, float r);
extern float func_001B1470(float a);
extern void func_001B5360(char *e, int st);
extern void func_001C24D0(char *e, char *q, char *out);
extern void func_001C2540(char *e, char *p, char *q, char *out);
extern int func_001C25E0(char *e, char *q);
extern int func_001C2770(char *e, char *c, int a);
extern void func_001C3D60(char *e, char *c);
extern void func_001EFFD0(int id, char *a, char *b, int k, float f);
extern void func_001FC580(char *e, int id);
extern char *D_00275B40;
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_00810350[];

void func_0012BE20(char *arg0, char *arg1) {
    int mode;
    int st;
    float t;

    mode = func_001C2770(arg0, arg1, 2);
    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(float *)(arg1 + 0xD8) = 0.0f;
        *(char *)(arg0 + 7) = 0;
        if (mode == 0) {
            *(float *)0x700038A0 = 0.0f;
            *(float *)0x700038A4 = -1.4f;
            *(float *)0x700038A8 = -5.0f;
            *(float *)0x700038AC = 1.0f;
            if (func_001C25E0(arg0, D_700038A0) == 0) {
                *(char *)(arg0 + 5) = 2;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 7) = 0;
            } else {
                *(float *)(arg0 + 0xB0) -= *(float *)(arg1 + 0x80) / 2.0f;
                *(float *)(arg0 + 0xB4) -= *(float *)(arg1 + 0x84) / 2.0f;
                *(float *)(arg0 + 0xB8) -= *(float *)(arg1 + 0x88) / 2.0f;
                *(float *)0x700038B0 = 0.0f;
                *(float *)0x700038B4 = -4.0f;
                *(float *)0x700038B8 = 0.0f;
                *(float *)0x700038BC = 1.0f;
                func_001C2540(arg0, arg1 + 0x60, D_700038B0, arg0 + 0xD0);
                if (*(int *)(arg1 + 0xE4) == 0x100) {
                    *(float *)0x700038B0 = 0.0f;
                    *(float *)0x700038B4 = 1.0f;
                    *(float *)0x700038B8 = -5.0f;
                    *(float *)0x700038BC = 1.0f;
                    func_001C24D0(arg0, D_700038B0, arg0 + 0xD0);
                    *(unsigned char *)(arg0 + 6) = 2;
                } else {
                    *(unsigned char *)(arg0 + 6) = 1;
                }
            }
        }
        break;
    case 1:
        func_0012D580(arg0, arg1, mode);
        break;
    case 2:
        func_00128830(arg0, st, 0.0f, 0.0f, -2.5f);
        func_001287F0(arg0, arg1, 0xD, 0.0f);
        *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        *(char *)(arg0 + 7) = 0;
        *(float *)(arg1 + 0xD8) = 0.0f;
        break;
    case 3:
        if (*(short *)(arg1 + 0xF4) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(char *)(arg0 + 7) = 0;
            func_00128830(arg0, st, 0.0f, 2.5f, 0.0f);
            func_001287F0(arg0, arg1, 0xE, 0.0f);
        } else {
            func_0012E2C0(arg0, arg1);
        }
        break;
    case 4:
        if (*(short *)(arg1 + 0xF4) & 0x5000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_00128830(arg0, st, 0.0f, 1.5f, 3.0f);
            func_001287F0(arg0, arg1, 0xF, 0.0f);
            *(short *)(arg1 + 0xD0) = 0x78;
            *(int *)(arg1 + 0xE4) = 0;
            *(float *)(arg1 + 0xF0) = 0.6f;
            *(float *)(arg0 + 0xC0) = -0.7853982f;
            *(float *)(arg1 + 0xD8) = 0.6f;
            func_001FC580(arg0, 0x1AA);
        }
        break;
    case 5:
        func_001B5360(arg0, st);
        if (*(int *)(arg1 + 0xE4) & 0xF) {
            if (func_00128640(arg0) == 0) {
                *(char *)(arg0 + 5) = 1;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 7) = 0;
            }
        } else {
            *(float *)(arg0 + 0xC0) = (3.1415927f * (75.0f * -*(float *)(arg1 + 0xF0))) / 180.0f;
            t = *(float *)(arg1 + 0xF0) - 0.04f;
            *(float *)(arg1 + 0xF0) = t;
            if (t < 0.0f) {
                *(float *)(arg0 + 0xC0) = 0.0f;
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                *(int *)(arg1 + 0xE4) = 0x400;
            }
            *(float *)(arg0 + 0xB4) += *(float *)(arg1 + 0xF0);
            if (*(short *)(arg1 + 0xF8) != 0x10) {
                t = *(float *)(arg0 + 0xC0);
                if (!(t < -0.17453294f) && t < 0.0f &&
                    func_001B13F0(D_00810350, arg0 + 0xB0, 50.0f) != 0) {
                    func_001287F0(arg0, arg1, 0x10, 0.0f);
                    *(float *)0x700038A0 = 0.838f;
                    *(float *)0x700038A4 = 0.42f;
                    *(float *)0x700038A8 = 0.0f;
                    *(float *)0x700038AC = 1.0f;
                    func_001026A0(D_700038A0, *(char **)(D_00275B40 + 0x14) + 0x90,
                                  D_700038A0);
                    *(float *)0x700038B0 = 0.0f;
                    *(float *)0x700038B4 = 0.0f;
                    *(float *)0x700038B8 = 1.0f;
                    *(float *)0x700038BC = 1.0f;
                    func_001026A0(D_700038B0, arg0 + 0xD0, D_700038B0);
                    if (*(unsigned char *)(arg1 + 0xE1) == 0) {
                        func_001EFFD0(0x80000008, D_700038A0, D_700038B0, 0xA, 0.7f);
                    } else {
                        func_001EFFD0(0x80000008, D_700038A0, D_700038B0, 0xB, 0.7f);
                    }
                }
            }
        }
        break;
    case 6:
        func_001B5360(arg0, st);
        if (*(int *)(arg1 + 0xE4) == 0x100) {
            func_001287F0(arg0, arg1, 0x11, 0.0f);
            *(float *)(arg0 + 0xC0) = 0.0f;
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(float *)(arg1 + 0xD8) = 0.0f;
            *(short *)(arg1 + 0xF4) = 0;
        } else if ((*(int *)(arg1 + 0xE4) & 0xF) && func_00128640(arg0) == 0) {
            *(char *)(arg0 + 5) = 1;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 7) = 0;
        }
        break;
    case 7:
        if (*(short *)(arg1 + 0xF4) & 0x1000) {
            func_00128830(arg0, st, 0.0f, 0.0f, 1.0f);
            func_001287F0(arg0, arg1, 0, 0.0f);
            func_001287F0(arg0, arg1, 1, 6.0f);
            *(float *)(arg0 + 0xC4) = func_001B1470(3.1415927f + *(float *)(arg0 + 0xC4));
            if (func_00128640(arg0) == 0) {
                *(char *)(arg0 + 5) = 1;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 7) = 0;
            }
        }
        break;
    }
    if (mode == 0) {
        func_001C3D60(arg0, arg1);
    }
}
