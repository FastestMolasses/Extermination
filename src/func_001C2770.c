// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031D0 0x70003610 0x70003614 0x70003618 0x7000361C 0x70003620 0x70003628 0x700038A0 0x700038A4 0x700038A8 0x700038AC 0x700038B0 0x700038B4 0x700038B8 0x700038BC 0x700038C0 0x700038C4 0x700038C8 0x700038CC 0x700038D0 0x700038D4 0x700038D8 0x700038DC


// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 98.27% NEARMISS).
// Fixes read from the original instructions:
//  - Every scratchpad word is a relocated extern (idiom-32, opted in with
//    // SPAD:), one extern per address, volatile; the vec4 slots are arrays
//    whose element 0 is the base word, and the hit-record pointer at
//    0x700031D0 is `char *volatile D_700031D0`. This removed the hoisted
//    0x7000 upper halves the literal spelling speculated into delay slots.
//  - The pointer parameters of the callees are void * and the act/st field
//    addresses are passed uncast (the casts reordered the argument setup);
//    only the second argument of the func_001031E0(act + 0x100, act + 0xB0)
//    copy keeps a float * cast, which gives the original's argument order.
//  - The blend step is a compound `+= 0.1f` on st+0xD4, then re-read.
//  - In the sub-state 4 fallback "slide onto a plain wall" arm, latch = 5 is
//    assigned after the state word is masked (statement order).

//
// SEMANTICS: player wall/ledge interaction state machine ("climb / vault /
// hang / drop" probe + resolve), driven once per frame.
//   act = the player/actor block   (+0xB0 world position vec3,
//         +0xC0 pitch/lean angle, +0xC4 yaw, +0xD0 orientation quaternion,
//         +0x100 saved position used to blend the move out)
//   st  = the interaction state block (+0x60 fallback probe direction,
//         +0x70/+0x80 blend quaternions, +0x90 blended-target quaternion,
//         +0xC0 blended-target position, +0xD4 blend parameter 0..1,
//         +0xD8 remaining reach distance, +0xE4 packed state word,
//         +0xF0 vertical velocity while sliding down a wall)
//   flags = per-frame input mask: bit0 = "grab allowed", bit1 = "play the
//         grab-failed sound cue", bit2 = "stick to the surface".
//
//   st[0xE4] packs TWO state fields:
//     low nibble  (&0xF)  = phase, 0 = probing, 1..6 = blending to the
//                           latched pose, 7 = idle/exit.
//     byte 1      (>>8)   = probe sub-state (which capsule/ray set to cast).
//   The routine returns 1 when it consumed the frame (an interaction is
//   active) and 0 otherwise.
//
//   0x700038A0/0x700038B0/0x700038C0/0x700038D0 and 0x70003610/0x70003620
//   are the EE scratchpad vec4 staging slots used to build the probe
//   origin/direction pairs; 0x70003000 is the scratchpad quaternion result.
//   0x700031D0 holds the pointer to the last collision hit record
//     (+0x1A surface-flag halfword, +0x24..+0x2C the surface normal).
//   Surface flags: 0x8000 = climbable ledge, 0x2800 = vaultable/low wall,
//     0x5000 = slippery / non-grabbable.
//
// NOTE (s84): this symbol covers the splat false-split TAIL func_001C2FF0
// (merged via size:0x1278 in symbol_addrs.txt) — 0x1C2FF0 has zero jal
// references, it is a fall-through, not a callee.
//

extern float D_70003000[4];
extern volatile float D_70003610[4];
extern volatile float D_70003620[4];
extern volatile float D_700038A0[4];
extern volatile float D_700038B0[4];
extern volatile float D_700038C0[4];
extern volatile float D_700038D0[4];

extern volatile float D_70003614;
extern volatile float D_70003618;
extern volatile float D_7000361C;
extern volatile float D_70003628;
extern volatile float D_700038A4;
extern volatile float D_700038A8;
extern volatile float D_700038AC;
extern volatile float D_700038B4;
extern volatile float D_700038B8;
extern volatile float D_700038BC;
extern volatile float D_700038C4;
extern volatile float D_700038C8;
extern volatile float D_700038CC;
extern volatile float D_700038D4;
extern volatile float D_700038D8;
extern volatile float D_700038DC;
extern char *volatile D_700031D0;

extern void copy_qw4(void *dst, void *src);
extern void func_001026A0(void *dst, void *quat, void *vec);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001031E0(void *dst, void *src);
extern void func_00103230(void *dst, void *src, char *hit, float scale);
extern float func_0011E620(float y, float x);
extern int func_0019AB20(char *act, void *from, void *dir, int mask);
extern float func_001B1470(float angle);
extern int func_001C2540(char *act, void *origin, void *dir, void *quat);
extern int func_001C2690(char *act, void *origin, void *dir);
extern void func_001C39F0(char *act, char *st, float reach);
extern void func_001C3BE0(char *act, char *st);
extern void func_001C3DB0(void *a, void *b, void *c, void *out);
extern void func_001C9D50(void *dst, void *from, void *to, float t);
extern void func_001FBD50(char *act, int cue, int flags, float param);

int func_001C2770(char *act, char *st, int flags)
{
    int result;
    int phase;
    int sub;
    int state;
    int latch;
    int pull;
    int surf;
    char *hit;
    float f;

    state = *(int *)(st + 0xE4);
    result = 0;
    phase = state & 0xF;
    sub = state >> 8;

    switch (phase) {
    case 0:
        latch = 0;
        func_001C39F0(act, st, *(float *)(st + 0xD8));
        D_700038A0[0] = 0.0f;
        D_700038A4 = 2.0f;
        D_700038A8 = 4.0f;
        D_700038AC = 1.0f;

        switch (sub) {
        case 0:
            D_700038B0[0] = 0.0f;
            D_700038B4 = 0.0f;
            D_700038B8 = 6.0f;
            D_700038BC = 1.0f;
            if (func_001C2540(act, D_700038A0, D_700038B0, act + 0xD0) != 0) {
                surf = *(short *)(D_700031D0 + 0x1A);
                if (surf & 0x8000) {
                    latch = 1;
                    result = 1;
                    *(int *)(st + 0xE4) &= 0xF0;
                    *(float *)(act + 0xC0) = 0.0f;
                    *(int *)(st + 0xE4) |= 3;
                    *(float *)(st + 0xD8) = 0.0f;
                } else if (surf & 0x2800) {
                    latch = 1;
                    result = 1;
                    *(int *)(st + 0xE4) &= 0xF0;
                    *(float *)(act + 0xC0) = 0.0f;
                    *(int *)(st + 0xE4) |= 2;
                    *(float *)(st + 0xD8) = 0.0f;
                }
            } else {
                D_700038B0[0] = 0.0f;
                D_700038B4 = -4.0f;
                D_700038B8 = 0.0f;
                D_700038BC = 1.0f;
                func_001C2540(act, st + 0x60, D_700038B0, act + 0xD0);
            }
            break;

        case 1:
        case 2:
        case 3:
            if (*(float *)(st + 0xD8) != 0.0f) {
                D_700038B0[0] = 0.0f;
                D_700038B4 = 0.0f;
                D_700038B8 = 6.0f;
                D_700038BC = 1.0f;
                if (func_001C2540(act, D_700038A0, D_700038B0, act + 0xD0) != 0) {
                    if (flags & 1) {
                        switch (sub) {
                        case 1:
                            if (*(short *)(D_700031D0 + 0x1A) & 0x5000) {
                                break;
                            }
                            D_700038B0[0] = 0.0f;
                            D_700038B4 = -4.0f;
                            D_700038B8 = 0.0f;
                            D_700038BC = 1.0f;
                            func_001C2540(act, st + 0x60, D_700038B0, act + 0xD0);
                            return 8;
                        case 2:
                            if (*(short *)(D_700031D0 + 0x1A) & 0x2800) {
                                break;
                            }
                            D_700038B0[0] = 0.0f;
                            D_700038B4 = -4.0f;
                            D_700038B8 = 0.0f;
                            D_700038BC = 1.0f;
                            func_001C2540(act, st + 0x60, D_700038B0, act + 0xD0);
                            return 8;
                        case 3:
                            if (*(short *)(D_700031D0 + 0x1A) & 0x8000) {
                                break;
                            }
                            /* fallthrough */
                        default:
                            D_700038B0[0] = 0.0f;
                            D_700038B4 = -4.0f;
                            D_700038B8 = 0.0f;
                            D_700038BC = 1.0f;
                            func_001C2540(act, st + 0x60, D_700038B0, act + 0xD0);
                            return 8;
                        }
                    }
                    latch = 1;
                    *(int *)(st + 0xE4) &= 0xF0;
                    surf = *(short *)(D_700031D0 + 0x1A);
                    result = 1;
                    if (surf & 0x8000) {
                        *(int *)(st + 0xE4) |= 3;
                    } else if (surf & 0x2800) {
                        *(int *)(st + 0xE4) |= 2;
                    } else {
                        *(int *)(st + 0xE4) |= 1;
                    }
                } else {
                    D_700038B0[0] = 0.0f;
                    D_700038B4 = -4.0f;
                    D_700038B8 = 0.0f;
                    D_700038BC = 1.0f;
                    if (func_001C2540(act, st + 0x60, D_700038B0, act + 0xD0) != 0) {
                        latch = 2;
                    } else {
                        D_700038C0[0] = 0.0f;
                        D_700038C4 = -2.0f;
                        D_700038C8 = 0.0f;
                        D_700038CC = 1.0f;
                        func_001026A0(D_700038C0, act + 0xD0, D_700038C0);
                        func_001028B8(D_700038C0, D_700038C0, act + 0xB0);
                        D_700038D0[0] = 0.0f;
                        D_700038D4 = -5.0f;
                        D_700038D8 = 0.0f;
                        D_700038DC = 0.0f;
                        if (func_0019AB20(act, D_700038C0, D_700038D0, 7) == 0) {
                            D_700038A0[0] = 0.0f;
                            D_700038B0[0] = 0.0f;
                            D_700038B4 = 0.0f;
                            D_700038A4 = -3.0f;
                            D_700038A8 = -1.0f;
                            D_700038AC = 1.0f;
                            D_700038B8 = -6.0f;
                            D_700038BC = 1.0f;
                            if (func_001C2540(act, D_700038A0, D_700038B0, act + 0xD0) != 0) {
                                latch = 3;
                                *(int *)(st + 0xE4) &= 0xF0;
                                surf = *(short *)(D_700031D0 + 0x1A);
                                result = 1;
                                if (surf & 0x8000) {
                                    *(int *)(st + 0xE4) |= 3;
                                } else if (surf & 0x2800) {
                                    *(int *)(st + 0xE4) |= 2;
                                } else {
                                    *(int *)(st + 0xE4) |= 1;
                                }
                            } else {
                                latch = 0x11;
                                result = 1;
                                *(int *)(st + 0xE4) &= 0xF0;
                                *(int *)(st + 0xE4) |= 4;
                                *(float *)(st + 0xF0) = 0.0f;
                            }
                        }
                    }
                }
            }
            break;

        case 4:
            *(float *)(act + 0xC0) = (3.1415927f * (56.25f * -*(float *)(st + 0xF0))) / 180.0f;
            /* fallthrough */
        case 5:
        case 6:
            if (sub == 6) {
                *(float *)(st + 0xF0) = *(float *)(st + 0xF0) - 0.06f;
            } else {
                *(float *)(st + 0xF0) = *(float *)(st + 0xF0) - 0.04f;
            }
            if (*(float *)(st + 0xF0) < -0.8f) {
                *(float *)(st + 0xF0) = -0.8f;
            }
            *(float *)(act + 0xB4) += *(float *)(st + 0xF0);

            if (sub == 4) {
                D_700038B0[0] = 0.0f;
                D_700038B4 = 0.0f;
                D_700038B8 = 8.0f;
                D_700038BC = 1.0f;
                if (func_001C2540(act, D_700038A0, D_700038B0, act + 0xD0) != 0) {
                    hit = D_700031D0;
                    surf = *(short *)(hit + 0x1A);
                    if (surf & 0x8000) {
                        if (!(flags & 4)) {
                            *(float *)(st + 0xD8) = 0.0f;
                            latch = 1;
                            result = 1;
                            *(int *)(st + 0xE4) &= 0xF0;
                            *(float *)(act + 0xC0) = 0.0f;
                            *(int *)(st + 0xE4) |= 3;
                        }
                    } else if (surf & 0x2800) {
                        if (flags & 4) {
                            D_70003610[0] = *(float *)(hit + 0x24);
                            D_70003614 = *(float *)(hit + 0x28);
                            D_70003618 = *(float *)(hit + 0x2C);
                            D_7000361C = 1.0f;
                            func_00103230(D_70003610, D_70003610, hit, 0.2f);
                            func_001028B8(act + 0xB0, D_70003610, act + 0xB0);
                        } else {
                            *(float *)(st + 0xD8) = 0.0f;
                            latch = 1;
                            result = 1;
                            *(int *)(st + 0xE4) &= 0xF0;
                            *(float *)(act + 0xC0) = 0.0f;
                            *(int *)(st + 0xE4) |= 2;
                        }
                    } else {
                        *(float *)(st + 0xD8) = 0.0f;
                        *(int *)(st + 0xE4) &= 0xF0;
                        if (flags & 2) {
                            latch = 9;
                            result = 1;
                            *(int *)(st + 0xE4) |= 1;
                            func_001FBD50(act, 0x1AC, 0, 300.0f);
                        } else {
                            *(float *)(act + 0xC0) = 0.0f;
                            latch = 1;
                            *(int *)(st + 0xE4) |= 1;
                            result = 1;
                            func_001FBD50(act, 0x1AC, 0, 300.0f);
                        }
                    }
                } else {
                    D_700038B0[0] = 0.0f;
                    D_700038B4 = -4.0f;
                    D_700038B8 = 0.0f;
                    D_700038BC = 1.0f;
                    if (func_001C2540(act, st + 0x60, D_700038B0, act + 0xD0) != 0) {
                        if (*(short *)(D_700031D0 + 0x1A) & 0x2800) {
                            if (!(flags & 4)) {
                                *(float *)(st + 0xD8) = 0.0f;
                                latch = 5;
                                result = 1;
                                *(int *)(st + 0xE4) &= 0xF0;
                                *(int *)(st + 0xE4) |= 2;
                                *(float *)(act + 0xC0) = 0.0f;
                            }
                        } else {
                            *(float *)(st + 0xD8) = 0.0f;
                            *(int *)(st + 0xE4) &= 0xF0;
                            latch = 5;
                            result = 1;
                            *(int *)(st + 0xE4) |= 1;
                            *(float *)(act + 0xC0) = 0.0f;
                            func_001FBD50(act, 0x1AC, 0, 300.0f);
                        }
                    }
                }
            } else if (sub == 5) {
                D_700038B0[0] = 0.0f;
                D_700038B4 = 0.0f;
                D_700038B8 = 8.0f;
                D_700038BC = 1.0f;
                if (func_001C2540(act, D_700038A0, D_700038B0, act + 0xD0) != 0 &&
                    *(float *)(st + 0xF0) < 0.0f) {
                    hit = D_700031D0;
                    surf = *(short *)(hit + 0x1A);
                    if (surf & 0x5000) {
                        *(float *)(st + 0xD8) = 0.0f;
                        *(int *)(st + 0xE4) &= 0xF0;
                        if (flags & 2) {
                            latch = 9;
                            result = 1;
                            *(int *)(st + 0xE4) |= 1;
                            func_001FBD50(act, 0x1AC, 0, 300.0f);
                        } else {
                            *(float *)(act + 0xC0) = 0.0f;
                            latch = 1;
                            *(int *)(st + 0xE4) |= 1;
                            result = 1;
                            func_001FBD50(act, 0x1AC, 0, 300.0f);
                        }
                        break;
                    }
                    if (surf & 0x2800) {
                        D_70003610[0] = *(float *)(hit + 0x24);
                        D_70003614 = *(float *)(hit + 0x28);
                        D_70003618 = *(float *)(hit + 0x2C);
                        D_7000361C = 1.0f;
                        func_00103230(D_70003610, D_70003610, hit, 0.2f);
                        func_001028B8(act + 0xB0, D_70003610, act + 0xB0);
                    }
                }
                {
                    D_700038B0[0] = 0.0f;
                    D_700038B4 = -4.0f;
                    D_700038B8 = 0.0f;
                    D_700038BC = 1.0f;
                    if (func_001C2540(act, st + 0x60, D_700038B0, act + 0xD0) != 0 &&
                        *(float *)(st + 0xF0) < 0.0f &&
                        (*(short *)(D_700031D0 + 0x1A) & 0x5000)) {
                        *(float *)(st + 0xD8) = 0.0f;
                        *(float *)(act + 0xC0) = 0.0f;
                        latch = 5;
                        *(int *)(st + 0xE4) &= 0xF0;
                        result = 1;
                        *(int *)(st + 0xE4) |= 1;
                        func_001FBD50(act, 0x1AC, 0, 300.0f);
                    }
                }
            } else {
                D_700038B0[0] = 0.0f;
                D_700038B4 = 0.0f;
                D_700038B8 = 8.0f;
                D_700038BC = 1.0f;
                if (func_001C2690(act, D_700038A0, D_700038B0) != 0 &&
                    *(float *)(st + 0xF0) < 0.0f) {
                    hit = D_700031D0;
                    surf = *(short *)(hit + 0x1A);
                    if (surf & 0x5000) {
                        *(float *)(st + 0xD8) = 0.0f;
                        *(int *)(st + 0xE4) &= 0xF0;
                        if (flags & 2) {
                            latch = 9;
                            result = 1;
                            *(int *)(st + 0xE4) |= 1;
                            func_001FBD50(act, 0x1AC, 0, 300.0f);
                        } else {
                            *(float *)(act + 0xC0) = 0.0f;
                            latch = 1;
                            *(int *)(st + 0xE4) |= 1;
                            result = 1;
                            func_001FBD50(act, 0x1AC, 0, 300.0f);
                        }
                        break;
                    }
                    if (surf & 0x2800) {
                        D_70003610[0] = *(float *)(hit + 0x24);
                        D_70003614 = *(float *)(hit + 0x28);
                        D_70003618 = *(float *)(hit + 0x2C);
                        D_7000361C = 1.0f;
                        func_00103230(D_70003610, D_70003610, hit, 0.2f);
                        func_001028B8(act + 0xB0, D_70003610, act + 0xB0);
                    }
                }
                {
                    D_700038B0[0] = 0.0f;
                    D_700038B4 = -4.0f;
                    D_700038B8 = 0.0f;
                    D_700038BC = 1.0f;
                    if (func_001C2690(act, st + 0x60, D_700038B0) != 0 &&
                        *(float *)(st + 0xF0) < 0.0f &&
                        (*(short *)(D_700031D0 + 0x1A) & 0x5000)) {
                        *(int *)(st + 0xE4) &= 0xF0;
                        *(float *)(st + 0xD8) = 0.0f;
                        if (flags & 2) {
                            latch = 9;
                            result = 1;
                            *(int *)(st + 0xE4) |= 1;
                            func_001FBD50(act, 0x1AC, 0, 300.0f);
                        } else {
                            *(float *)(act + 0xC0) = 0.0f;
                            latch = 5;
                            result = 1;
                            *(int *)(st + 0xE4) |= 1;
                            func_001FBD50(act, 0x1AC, 0, 300.0f);
                        }
                    }
                }
            }
            break;

        case 7:
        default:
            break;
        }

        /* ---- shared tail: commit the latched interaction ---- */
        if (latch != 0) {
            if (latch & 0x10) {
                D_70003610[0] = 0.0f;
                D_70003614 = 1.0f;
                D_70003618 = 0.0f;
                D_7000361C = 1.0f;
                func_001C3DB0(st + 0x80, D_70003610, st + 0x70, D_70003620);
                *(float *)(act + 0xC4) = func_001B1470(*(float *)(act + 0xC4) +
                    func_0011E620(D_70003620[0], D_70003628));
                *(float *)(st + 0x70) = 0.0f;
                *(float *)(st + 0x74) = 0.0f;
                *(float *)(st + 0x78) = 1.0f;
                *(float *)(st + 0x7C) = 1.0f;
                *(int *)(st + 0x80) = 0;
                *(float *)(st + 0x84) = 1.0f;
                latch = 5;
                *(int *)(st + 0x88) = 0;
            } else {
                hit = D_700031D0;
                D_70003610[0] = *(float *)(hit + 0x24);
                D_70003614 = *(float *)(hit + 0x28);
                D_70003618 = *(float *)(hit + 0x2C);
                D_7000361C = 1.0f;
                func_001C3DB0(st + 0x80, D_70003610, st + 0x70, D_70003620);
                if (*(float *)(D_700031D0 + 0x28) == 1.0f) {
                    *(float *)(act + 0xC4) = func_001B1470(*(float *)(act + 0xC4) +
                        func_0011E620(D_70003620[0], D_70003628));
                    *(float *)(st + 0x70) = 0.0f;
                    *(float *)(st + 0x74) = 0.0f;
                    *(float *)(st + 0x78) = 1.0f;
                    *(float *)(st + 0x7C) = 1.0f;
                } else {
                    func_001031E0(st + 0x70, D_70003620);
                }
                *(float *)(st + 0x80) = *(float *)(D_700031D0 + 0x24);
                *(float *)(st + 0x84) = *(float *)(D_700031D0 + 0x28);
                *(float *)(st + 0x88) = *(float *)(D_700031D0 + 0x2C);
            }
        }
        pull = latch & 1;

        if (pull != 0) {
            func_001C3BE0(act, st);
            if (latch & 8) {
                D_700038A0[0] = 0.0f;
                D_700038A4 = 2.0f;
                D_700038A8 = 4.0f;
                D_700038AC = 1.0f;
            } else if (latch & 4) {
                D_700038A0[0] = 0.0f;
                D_700038A4 = 0.0f;
                D_700038A8 = 0.0f;
                D_700038AC = 1.0f;
            } else if (latch & 2) {
                D_700038A0[0] = 0.0f;
                D_700038A4 = -4.0f;
                D_700038A8 = -0.7f;
                D_700038AC = 1.0f;
            } else {
                D_700038A0[0] = 0.0f;
                D_700038A4 = 3.0f;
                D_700038A8 = 4.0f;
                D_700038AC = 1.0f;
            }

            if (latch & 8) {
                func_001026A0(D_700038A0, act + 0xD0, D_700038A0);
                *(float *)(act + 0xB0) = *(float *)(act + 0xB0) + D_700038A0[0];
                *(float *)(act + 0xB4) = *(float *)(act + 0xB4) + D_700038A4;
                *(float *)(act + 0xB8) = *(float *)(act + 0xB8) + D_700038A8;
                copy_qw4(act + 0xD0, D_70003000);
                *(int *)(st + 0xE4) = 0x100;
                result = 0;
            } else {
                func_001026A0(D_700038A0, act + 0xD0, D_700038A0);
                D_700038A0[0] = D_700038A0[0] + *(float *)(act + 0xB0);
                D_700038A4 = D_700038A4 + *(float *)(act + 0xB4);
                D_700038A8 = D_700038A8 + *(float *)(act + 0xB8);
                D_700038AC = 1.0f;
                copy_qw4(st + 0x90, D_70003000);
                func_001031E0(st + 0xC0, D_700038A0);
                func_001031E0(act + 0x100, (float *)(act + 0xB0));
                copy_qw4(D_70003000, act + 0xD0);
                *(float *)(st + 0xD4) = 0.0f;
            }
        }
        break;

    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        /* blending the actor into the latched pose over 10 frames */
        result = 1;
        func_001C9D50(D_70003000, act + 0xD0, st + 0x90,
                      *(float *)(st + 0xD4));
        *(float *)(st + 0xD4) += 0.1f;
        f = *(float *)(st + 0xD4);
        if (!(f <= 1.0f)) {
            copy_qw4(D_70003000, st + 0x90);
            copy_qw4(act + 0xD0, st + 0x90);
            *(float *)(act + 0xB0) = *(float *)(act + 0x100);
            *(float *)(act + 0xB4) = *(float *)(act + 0x104);
            *(float *)(act + 0xB8) = *(float *)(act + 0x108);
            *(float *)(act + 0x100) = 0.0f;
            *(float *)(act + 0x104) = 0.0f;
            *(float *)(act + 0x108) = 0.0f;
            *(int *)(st + 0xE4) = phase << 8;
        }
        break;

    case 7:
    default:
        break;
    }
    return result;
}
