// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SEMANTICS: 9-state per-frame actor behaviour state machine, dispatched on the
// state byte at arg0[6] via jtbl_0026D0A0 (bound sltiu 0x9). arg0 = actor,
// arg1 = the AI/target block. func_001C2770(actor, target, 2) runs first and
// returns a "busy" flag that gates both state 0 and the tail func_001C3D60.
//   0: clear target+0xD8 / actor[7]; if not busy, seed the scratch vec4 at
//      0x700038A0 = (0,-1.4,-5,1) and probe with func_001C25E0; on failure go to
//      actor[5]=2, else pull the actor back by half the target velocity
//      (target+0x80..0x88), aim with func_001C2540 via (0,-4,0,1), then advance
//      to state 2 (with an extra func_001C24D0 pass on (0,1,-5,1)) when
//      target+0xE4 == 0x100, otherwise to state 1.
//   1: delegate to func_0012D580(actor, target, busy).
//   2: hop (0,0,-2.5), anim 0x13, advance, clear target+0xD8, sfx 0x1AE @300.
//   3: when target+0xF4 & 0x1000: advance, hop (0,2.5,0), anim 0x14;
//      otherwise func_0012E2C0.
//   4: when target+0xF4 & 0x5000: advance, hop (0,1.5,3), anim 0x15, arm the
//      spin (target+0xD0 = 0x78, +0xE4 = 0, +0xF0 = +0xD8 = 0.6f,
//      actor+0xC0 = -pi/4) and sfx 0x1AA.
//   5: func_001B5360 + func_0012E260; if target+0xE4 & 0xF, bail to actor[5]=1
//      when func_00128640 fails; else drive the pitch actor+0xC0 from
//      -target+0xF0 (75 deg scaled to radians), decay +0xF0 by 0.04 (advance
//      state and clear +0xD0 when it goes negative), add it to actor+0xB4 and
//      switch to actor[5]=6 when func_0012E0B0 reports done.
//   6: same guard; spin actor+0xB4 by 0.1 * cos(func_001B1470(target+0xD0)),
//      step +0xD0 by 0x18 and advance once it passes 0x2D0 (setting +0xE4=0x400).
//   7: func_001B5360; on target+0xE4 == 0x100 play anim 0x11, clear actor+0xC0,
//      advance, clear target+0xD8/+0xF4; else the +0xE4 & 0xF bail-out.
//   8: when target+0xF4 & 0x1000: hop (0,0,1), anims 0 and 1 @6.0, flip the yaw
//      actor+0xC4 by pi through func_001B1470, then the bail-out check.
// Tail: if not busy, func_001C3D60(actor, target).
extern int func_001C2770(void *a0, void *a1, int a2);
extern int func_001C25E0(void *a0, void *a1);
extern void func_001C2540(void *a0, void *a1, void *a2, void *a3);
extern void func_001C24D0(void *a0, void *a1, void *a2);
extern void func_001C3D60(void *a0, void *a1);
extern void func_0012D580(void *a0, void *a1, int a2);
extern void func_00128830(void *a0, float f0, float f1, float f2);
extern void func_001287F0(void *a0, void *a1, int a2, float f0);
extern void func_001FBD50(void *a0, int a1, int a2, float f0);
extern void func_001FC580(void *a0, int a1);
extern void func_0012E2C0(void *a0, void *a1);
extern void func_001B5360(void *a0);
extern void func_0012E260(void *a0);
extern int func_00128640(void *a0);
extern int func_0012E0B0(void *a0, void *a1);
extern float func_001B1470(float f0);
extern float func_0011E2A8(float f0);
extern int D_700038A0;
extern int D_700038B0;

void func_0012C490(unsigned char *arg0, unsigned char *arg1) {
    int busy;
    int st;
    float t;
    int e;

    busy = func_001C2770(arg0, arg1, 2);
    st = arg0[6];
    switch (st) {
    case 0:
        *(int *)(arg1 + 0xD8) = 0;
        arg0[7] = 0;
        if (busy == 0) {
            *(volatile int *)0x700038A0 = 0;
            *(volatile int *)0x700038A4 = 0xBFB33333;
            *(volatile int *)0x700038A8 = 0xC0A00000;
            *(volatile int *)0x700038AC = 0x3F800000;
            if (func_001C25E0(arg0, &D_700038A0) == 0) {
                arg0[5] = 2;
                arg0[6] = 0;
                arg0[7] = 0;
            } else {
                *(float *)(arg0 + 0xB0) -= *(float *)(arg1 + 0x80) / 2.0f;
                *(float *)(arg0 + 0xB4) -= *(float *)(arg1 + 0x84) / 2.0f;
                *(float *)(arg0 + 0xB8) -= *(float *)(arg1 + 0x88) / 2.0f;
                *(volatile int *)0x700038B0 = 0;
                *(volatile int *)0x700038B4 = 0xC0800000;
                *(volatile int *)0x700038B8 = 0;
                *(volatile int *)0x700038BC = 0x3F800000;
                func_001C2540(arg0, arg1 + 0x60, &D_700038B0, arg0 + 0xD0);
                if (*(int *)(arg1 + 0xE4) == 0x100) {
                    *(volatile int *)0x700038B0 = 0;
                    *(volatile int *)0x700038B4 = 0x3F800000;
                    *(volatile int *)0x700038B8 = 0xC0A00000;
                    *(volatile int *)0x700038BC = 0x3F800000;
                    func_001C24D0(arg0, &D_700038B0, arg0 + 0xD0);
                    arg0[6] = 2;
                } else {
                    arg0[6] = 1;
                }
            }
        }
        break;
    case 1:
        func_0012D580(arg0, arg1, busy);
        break;
    case 2:
        func_00128830(arg0, 0.0f, 0.0f, -2.5f);
        func_001287F0(arg0, arg1, 0x13, 0.0f);
        arg0[6] = arg0[6] + 1;
        arg0[7] = 0;
        *(int *)(arg1 + 0xD8) = 0;
        func_001FBD50(arg0, 0x1AE, 0, 300.0f);
        break;
    case 3:
        if (*(short *)(arg1 + 0xF4) & 0x1000) {
            arg0[6] = st + 1;
            arg0[7] = 0;
            func_00128830(arg0, 0.0f, 2.5f, 0.0f);
            func_001287F0(arg0, arg1, 0x14, 0.0f);
        } else {
            func_0012E2C0(arg0, arg1);
        }
        break;
    case 4:
        if (*(short *)(arg1 + 0xF4) & 0x5000) {
            arg0[6] = st + 1;
            func_00128830(arg0, 0.0f, 1.5f, 3.0f);
            func_001287F0(arg0, arg1, 0x15, 0.0f);
            *(short *)(arg1 + 0xD0) = 0x78;
            *(int *)(arg1 + 0xE4) = 0;
            *(int *)(arg1 + 0xF0) = 0x3F19999A;
            *(int *)(arg0 + 0xC0) = 0xBF490FDB;
            *(int *)(arg1 + 0xD8) = 0x3F19999A;
            func_001FC580(arg0, 0x1AA);
        }
        break;
    case 5:
        func_001B5360(arg0);
        func_0012E260(arg0);
        if (*(int *)(arg1 + 0xE4) & 0xF) {
            if (func_00128640(arg0) == 0) {
                arg0[5] = 1;
                arg0[6] = 0;
                arg0[7] = 0;
            }
        } else {
            *(float *)(arg0 + 0xC0) = 3.1415927f * (75.0f * -*(float *)(arg1 + 0xF0)) / 180.0f;
            t = *(float *)(arg1 + 0xF0) - 0.04f;
            *(float *)(arg1 + 0xF0) = t;
            if (t < 0.0f) {
                *(int *)(arg0 + 0xC0) = 0;
                arg0[6] = arg0[6] + 1;
                *(short *)(arg1 + 0xD0) = 0;
            }
            *(float *)(arg0 + 0xB4) += *(float *)(arg1 + 0xF0);
            if (func_0012E0B0(arg0, arg1) != 0) {
                *(int *)(arg0 + 0xC0) = 0;
                arg0[5] = 6;
                arg0[6] = 0;
            }
        }
        break;
    case 6:
        func_001B5360(arg0);
        func_0012E260(arg0);
        *(float *)(arg0 + 0xB4) += 0.1f * func_0011E2A8(func_001B1470((float)*(short *)(arg1 + 0xD0)));
        *(short *)(arg1 + 0xD0) = *(short *)(arg1 + 0xD0) + 0x18;
        if (*(int *)(arg1 + 0xE4) & 0xF) {
            if (func_00128640(arg0) == 0) {
                arg0[5] = 1;
                arg0[6] = 0;
                arg0[7] = 0;
            }
        } else if (func_0012E0B0(arg0, arg1) != 0) {
            *(int *)(arg0 + 0xC0) = 0;
            arg0[5] = 6;
            arg0[6] = 0;
        } else if (*(short *)(arg1 + 0xD0) > 0x2D0) {
            *(int *)(arg1 + 0xE4) = 0x400;
            arg0[6] = arg0[6] + 1;
        }
        break;
    case 7:
        func_001B5360(arg0);
        e = *(int *)(arg1 + 0xE4);
        if (e == 0x100) {
            func_001287F0(arg0, arg1, 0x11, 0.0f);
            *(int *)(arg0 + 0xC0) = 0;
            arg0[6] = arg0[6] + 1;
            *(int *)(arg1 + 0xD8) = 0;
            *(short *)(arg1 + 0xF4) = 0;
        } else if (e & 0xF) {
            if (func_00128640(arg0) == 0) {
                arg0[5] = 1;
                arg0[6] = 0;
                arg0[7] = 0;
            }
        }
        break;
    case 8:
        if (*(short *)(arg1 + 0xF4) & 0x1000) {
            func_00128830(arg0, 0.0f, 0.0f, 1.0f);
            func_001287F0(arg0, arg1, 0, 0.0f);
            func_001287F0(arg0, arg1, 1, 6.0f);
            *(float *)(arg0 + 0xC4) = func_001B1470(3.1415927f + *(float *)(arg0 + 0xC4));
            if (func_00128640(arg0) == 0) {
                arg0[5] = 1;
                arg0[6] = 0;
                arg0[7] = 0;
            }
        }
        break;
    }
    if (busy == 0) {
        func_001C3D60(arg0, arg1);
    }
}
