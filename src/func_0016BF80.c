// NEARMISS func_0016BF80  (vram 0x0016BF80, 0x5A0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.77% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Persistent a0/a1 register-role swap for the state byte vs the `arg0` pointer across nearly every case dispatch branch and helper call in the function (target keeps arg0 fixed in $a0 and colors state into $a1; our build swaps them), plus one dead-reload/branch-polarity difference in case 2's `==2`...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// State-machine step for an actor's "wade/swim" or similar timed action
// (dispatched on the state byte at arg0+6, states 0..9 plus a special
// state 0x14/20). State 0: kick clip 0x180 (func_001749A0), zero timers.
// State 1: gate on func_00224600 / D_00810700==0x15 door flag /
// D_00810E74 held-input mask, then func_0016BE40 and (once +0x38 hits 0)
// advance + fire clip 0x136. States 2/3: similar gating via func_0016BB80
// + func_0011DF78(+0x2E4) < 0.01 threshold, managing a queued sound handle
// at +0x31A/+0x31B/+0x31C and integrating the +0xB0/+0xB8/+0xB4 fields by
// dt*(+0x2E0/+0x2E8) each tick. State 4: decays +0x38 toward 0 by 0.15/tick.
// States 6/7/8: single-shot transitions gated on flag bits (+0x200 &
// 0x8000/0x1000) or a timer (+0x3C <= 23.0). State 9: kicks
// func_00179880/func_00175900 and on success resets to state 0. State 20
// (0x14) is a one-shot side dispatch to func_0021D2E0(arg0, 0x78, 1) that
// does NOT touch the state byte.
//
// NEARMISS: 93.77% (mwcc 2.3.3), 88.80% (pinned 991202). This is the
// idiom-20 branch-likely state dispatcher: writing the 11-way switch with
// ASCENDING case labels reproduces the target's descending beq/beql chain
// (0x14,9,8,7,6,5,4,3,2,1,0) including the case-9/5/0 beql slots. Residual
// is a persistent a0<->a1 register-role swap for the state byte vs `arg0`
// pointer (mwcc keeps `arg0` in $a0 and puts state in $a1; our build
// colors them the other way) plus one dead-reload/branch-shape difference
// in case 2's early-return (logically equivalent, different code layout).
extern int func_0011A070(int a0);
extern float func_0011DF78(float a);
extern int func_0016BB80(char *e);
extern void func_0016BE40(char *p);
extern void func_001749A0(int a, int b, float f);
extern void func_00174A50(char *p, float f);
extern int func_00175900(char *p, int f);
extern void func_00179880(char *p);
extern void func_00182870(char *e, int f);
extern int func_001FBD50(char *p, int a, int b, float f);
extern void func_0021D2E0(char *p, int a, int b);
extern int func_00224600(unsigned char st);
extern int D_00275B00[];
extern unsigned char D_00810700;
extern unsigned short D_00810E74;

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
        func_001749A0(0x180, 0, 8.0f);
        *(short *)(arg0 + 0x28) = 0;
        return;
    case 1:
        if (func_00224600(st) == 0) {
            if (D_00810700 == 0x15 && *(unsigned char *)0x70003B8D != 0) {
                *(unsigned char *)(arg0 + 6) = 5;
                return;
            }
            if (D_00810E74 & *(unsigned short *)0x70003B76) {
                *(unsigned char *)(arg0 + 6) = 5;
                return;
            }
            func_0016BE40(arg0);
            if (*(float *)(arg0 + 0x38) != 0.0f) {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                func_001FBD50(arg0, 0x136, 0, 300.0f);
                return;
            }
        }
        break;
    case 2:
        if (func_00224600(st) != 2) {
            if (*(unsigned char *)(arg0 + 0x25F) == 2) {
                return;
            }
            func_0016BB80(arg0);
            if (func_0011DF78(*(float *)(arg0 + 0x2E4)) < 0.01f) {
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
        break;
    case 3:
        if (func_00224600(st) == 2 || *(unsigned char *)(arg0 + 0x25F) == 2) {
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
        rc = func_00224600(st);
        if (rc != 2) {
            if (*(unsigned char *)(arg0 + 0x25F) == 2) {
                return;
            }
            if (func_0016BB80(arg0) == 0) {
                if (rc == 0) {
                    *(unsigned char *)(arg0 + 6) = 5;
                    return;
                }
            } else {
                f38 = *(float *)(arg0 + 0x38);
                if (f38 <= 0.15f) {
                    *(float *)(arg0 + 0x38) = 0.0f;
                    if (rc == 0) {
                        *(unsigned char *)(arg0 + 6) = 5;
                    }
                } else {
                    *(float *)(arg0 + 0x38) = f38 - 0.15f;
                }
                *(float *)(arg0 + 0xB0) += *(float *)(arg0 + 0x2E0) * *(float *)(arg0 + 0x38);
                *(float *)(arg0 + 0xB8) += *(float *)(arg0 + 0x2E8) * *(float *)(arg0 + 0x38);
                *(float *)(arg0 + 0xB4) = *(float *)0x700031B4 - 22.5f;
                return;
            }
        }
        break;
    case 5:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(signed char *)(arg0 + 0x1F0) = 0x2B;
        func_001749A0(0x181, 0, 8.0f);
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
            func_00174A50(arg0, 4.0f);
            return;
        }
        break;
    case 9:
        func_00179880(arg0);
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
