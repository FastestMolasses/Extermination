// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 16
//
// Per-frame state-machine step for an actor's timed "wade/pull" action, dispatched on the
// state byte at arg0+6 (states 0..9 plus the one-shot side state 0x14/20): state 0 kicks
// clip 0x180 and clears the timers, 1..4 gate on func_00224600 / the door+input globals and
// integrate the +0xB0/+0xB4/+0xB8 position fields by dt while managing the queued sound
// handle at +0x31A/+0x31B/+0x31C, 5..8 are single-shot flag/timer transitions, 9 finishes
// via func_00179880/func_00175900 and resets to state 0.

extern int func_0011A070(int a0);
extern float func_0011DF78(float a);
extern int func_0016BB80(char *e);
extern void func_0016BE40(char *p);
extern void func_001749A0(char *e, int id, int flags, float blend);
extern void func_00174A50(char *p, float f);
extern int func_00175900(char *p, int f);
extern void func_00179880(char *p, char *out);
extern void func_00182870(char *e, int f);
extern int func_001FBD50(char *p, int a, int b, float f);
extern void func_0021D2E0(char *p, int a, int b);
extern int func_00224600(void);
extern int D_00275B00[3];
extern unsigned char D_00810700[32];
extern unsigned short D_00810E74[32];

void func_0016BF80(char *arg0) {
    unsigned char st = *(unsigned char *)(arg0 + 6);
    int rc;
    signed char snd;
    float f38;

    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(signed char *)(arg0 + 7) = 0;
        *(int *)(arg0 + 0x2E4) = 0;
        *(float *)(arg0 + 0x38) = 0.0f;
        func_001749A0(arg0, 0x180, 0, 8.0f);
        *(short *)(arg0 + 0x28) = 0;
        return;
    case 1:
        if (func_00224600() == 0) {
            if (D_00810700[0] == 0x15 && *(unsigned char *)0x70003B8D != 0) {
                *(unsigned char *)(arg0 + 6) = 5;
                return;
            }
            if (D_00810E74[0] & *(unsigned short *)0x70003B76) {
                *(unsigned char *)(arg0 + 6) = 5;
                return;
            }
            func_0016BE40(arg0);
            if (*(float *)(arg0 + 0x38)) {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                func_001FBD50(arg0, 0x136, 0, 300.0f);
                return;
            }
        }
        break;
    case 2:
        if (func_00224600() == 2 || *(unsigned char *)(arg0 + 0x25F) == 2) {
            return;
        } else {
            func_0016BB80(arg0);
            if (func_0011DF78(*(float *)(arg0 + 0x2E4)) >= 0.01f) {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                snd = (signed char)func_001FBD50(arg0, 0x135, 0, 300.0f);
                *(signed char *)(arg0 + 0x31B) = snd;
                *(unsigned char *)(arg0 + 0x31A) = 1;
                *(unsigned short *)(arg0 + 0x31C) = 0x135;
            } else {
                func_0016BE40(arg0);
            }
            *(float *)(arg0 + 0xB0) += *(float *)(arg0 + 0x2E0) * *(float *)(arg0 + 0x38);
            *(float *)(arg0 + 0xB8) += *(float *)(arg0 + 0x2E8) * *(float *)(arg0 + 0x38);
            *(float *)(arg0 + 0xB4) = *(float *)0x700031B4 - 22.5f;
            return;
        }
    case 3:
        if (func_00224600() == 2 || *(unsigned char *)(arg0 + 0x25F) == 2) {
            snd = *(signed char *)(arg0 + 0x31B);
            if (snd != -1) {
                func_0011A070(snd);
                *(signed char *)(arg0 + 0x31B) = -1;
                *(unsigned char *)(arg0 + 0x31A) = 0;
                return;
            }
        } else {
            func_0016BB80(arg0);
            if (func_0011DF78(*(float *)(arg0 + 0x2E4)) < 0.01f) {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                snd = *(signed char *)(arg0 + 0x31B);
                if (snd != -1) {
                    func_0011A070(snd);
                    *(signed char *)(arg0 + 0x31B) = -1;
                    *(unsigned char *)(arg0 + 0x31A) = 0;
                }
                func_001FBD50(arg0, 0x136, 0, 300.0f);
            } else {
                func_0016BE40(arg0);
                if (*(unsigned char *)(arg0 + 0x31A) != 0 && *(unsigned short *)(arg0 + 0x31C) == 0x5DD && *(signed char *)(arg0 + 0x31B) == -1) {
                    *(signed char *)(arg0 + 0x31B) = (signed char)func_001FBD50(arg0, 0x135, 0, 300.0f);
                }
            }
            *(float *)(arg0 + 0xB0) += *(float *)(arg0 + 0x2E0) * *(float *)(arg0 + 0x38);
            *(float *)(arg0 + 0xB8) += *(float *)(arg0 + 0x2E8) * *(float *)(arg0 + 0x38);
            *(float *)(arg0 + 0xB4) = *(float *)0x700031B4 - 22.5f;
            return;
        }
        break;
    case 4:
        rc = func_00224600();
        if (rc == 2 || *(unsigned char *)(arg0 + 0x25F) == 2) {
            return;
        } else {
            if (func_0016BB80(arg0) == 0) {
                if (rc == 0) {
                    *(unsigned char *)(arg0 + 6) = 5;
                }
                return;
            }
            f38 = *(float *)(arg0 + 0x38);
            if (f38 > 0.15f) {
                *(float *)(arg0 + 0x38) = f38 - 0.15f;
            } else {
                *(float *)(arg0 + 0x38) = 0.0f;
                if (rc == 0) {
                    *(unsigned char *)(arg0 + 6) = 5;
                }
            }
            *(float *)(arg0 + 0xB0) += *(float *)(arg0 + 0x2E0) * *(float *)(arg0 + 0x38);
            *(float *)(arg0 + 0xB8) += *(float *)(arg0 + 0x2E8) * *(float *)(arg0 + 0x38);
            *(float *)(arg0 + 0xB4) = *(float *)0x700031B4 - 22.5f;
            return;
        }
    case 5:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(signed char *)(arg0 + 0x1F0) = 0x2B;
        func_001749A0(arg0, 0x181, 0, 8.0f);
        return;
    case 6:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            return;
        }
        break;
    case 7:
        if (*(float *)(arg0 + 0x3C) <= 23.0f) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            D_00275B00[2] = 1;
            func_001FBD50(arg0, 0x134, 0, 300.0f);
            return;
        }
        break;
    case 8:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(int *)(arg0 + 0x2EC) = 0;
            func_00174A50(arg0, 4.0f);
            return;
        }
        break;
    case 9:
        func_00179880(arg0, arg0 + 0x2EC);
        if (func_00175900(arg0, 1) != 0) {
            *(signed char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            *(signed char *)(arg0 + 0x1F0) = 0;
            func_00182870(arg0, 1);
            return;
        }
        return;
    case 20:
        func_0021D2E0(arg0, 0x78, 1);
        break;
    }
}
