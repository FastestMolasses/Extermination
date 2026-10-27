// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 86.77% NEARMISS).
// Fixes read from the original instructions:
//  - D_00275B40 is gp-relative: -sdatathreshold 4, with the absolute
//    0x00810xxx globals declared as arrays so they stay out of small data.
//  - func_001607D0 takes the entity pointer (its definition in src/ has one
//    parameter; the original passes arg0 through untouched in a0).
//  - float_to_int returns int (no sign-extension before the compare).
//  - The render tail is one if/else: `done` is cleared first, then
//    kind != 0x33 takes the dispatch+copy path, else only the dispatch;
//    the matrix pointer D_00275B40[4] is re-read for each float copy.

// Entity state machine (states 0/10-13; anim-turn-and-render dispatch).
// State 0 sets arg0+0x2E to 1 and, when func_001607D0(arg0) succeeds, renders
// and returns early; otherwise it marks 0x2F2 and may play sound 0x17A or move
// to state 10. States 10-13 count frames in 0x276, decrement D_00810CB0, play
// sound 0x5DC/0x5DD and gate on the pad latch D_00810CB6. Every path that
// leaves the switch runs the shared render tail: kind 0x33 only dispatches the
// anim matrix; other kinds dispatch then copy a matrix quadword block for kinds
// 0x32/0x35, sub-state 4 or the 0x2F2 flag, else copy the three position floats.
extern void anim_matrix_dispatch(char *self);
extern void copy_qw4(char *dst, char *src);
extern int float_to_int(float);
extern int func_001607D0(char *p);
extern void func_001FBD50(char *p, int a, int b, float f);
extern char **D_00275B40;
extern short D_00810CB0[];
extern unsigned char D_00810CB6[];
extern unsigned short D_00810E74[];

void func_001723D0(char *arg0) {
    unsigned char st;
    int done;

    st = *(unsigned char *)(arg0 + 7);
    switch (st) {
    case 0:
        if (D_00810CB6[0] == 1) {
            if (D_00810E74[0] & *(unsigned short *)0x70003B74) {
                D_00810CB6[0] = 2;
            }
            break;
        }
        if (D_00810CB6[0] != 0) {
            break;
        }
        *(short *)(arg0 + 0x2E) = 1;
        if (func_001607D0(arg0) != 0) {
            done = 0;
            if (*(unsigned char *)(arg0 + 0x1F0) != 0x33) {
                anim_matrix_dispatch(arg0);
                if (*(unsigned char *)(arg0 + 0x1F0) == 0x32 || *(unsigned char *)(arg0 + 0x1F0) == 0x35) {
                    copy_qw4(arg0 + 0x2A0, D_00275B40[4] + 0x90);
                    done = 1;
                } else if (*(unsigned char *)(arg0 + 0x275) == 4) {
                    copy_qw4(arg0 + 0x2A0, D_00275B40[4] + 0x90);
                    done = 1;
                } else if (*(unsigned char *)(arg0 + 0x2F2) != 0) {
                    copy_qw4(arg0 + 0x2A0, D_00275B40[4] + 0x90);
                    done = 1;
                }
                if (done == 0) {
                    *(float *)(arg0 + 0x2D0) = *(float *)(D_00275B40[4] + 0xC0);
                    *(float *)(arg0 + 0x2D4) = *(float *)(D_00275B40[4] + 0xC4);
                    *(float *)(arg0 + 0x2D8) = *(float *)(D_00275B40[4] + 0xC8);
                }
            } else {
                anim_matrix_dispatch(arg0);
            }
            return;
        }
        *(unsigned char *)(arg0 + 0x2F2) = 1;
        if (*(unsigned char *)(arg0 + 0x274) != 0) {
            *(short *)(arg0 + 0x2E) = 0;
            if (D_00810CB0[0] == 0) {
                *(unsigned char *)(arg0 + 0x274) = 0;
                func_001FBD50(arg0, 0x17A, 0, 300.0f);
            } else {
                *(unsigned char *)(arg0 + 7) = 0xA;
            }
        }
        break;
    case 10:
        *(unsigned char *)(arg0 + 7) = st + 1;
        *(unsigned char *)(arg0 + 0x274) = 0;
        *(unsigned char *)(arg0 + 0x2F2) = 0;
        /* fallthrough */
    case 11:
        *(short *)(arg0 + 0x276) = *(short *)(arg0 + 0x276) + 1;
        if (*(short *)(arg0 + 0x276) >= 0xB) {
            *(unsigned char *)(arg0 + 7) = *(unsigned char *)(arg0 + 7) + 1;
            *(short *)(*(char **)(arg0 + 0x20) + 0x2E) = 1;
            D_00810CB0[0] = D_00810CB0[0] - 1;
            if (*(unsigned char *)(arg0 + 0x1F0) == 0x31 || *(unsigned char *)(arg0 + 0x1F0) == 0x34) {
                func_001FBD50(arg0, 0x5DC, 0, 300.0f);
            } else {
                func_001FBD50(arg0, 0x5DD, 0, 300.0f);
            }
            D_00810CB6[0] = 1;
        }
        break;
    case 12:
        if (D_00810CB6[0] == 1 && (D_00810E74[0] & *(unsigned short *)0x70003B74)) {
            D_00810CB6[0] = 2;
        }
        *(short *)(arg0 + 0x276) = *(short *)(arg0 + 0x276) + 1;
        if (*(short *)(arg0 + 0x276) >= float_to_int(*(float *)(arg0 + 0x2F4))) {
            *(short *)(arg0 + 0x276) = 0;
            *(unsigned char *)(arg0 + 0x2F2) = 1;
            if (D_00810CB6[0] == 0) {
                if (D_00810CB0[0] != 0) {
                    *(unsigned char *)(arg0 + 0x1F0) = 0x33;
                    *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                    *(unsigned char *)(arg0 + 7) = 0;
                } else {
                    *(unsigned char *)(arg0 + 7) = 0;
                }
            } else {
                *(unsigned char *)(arg0 + 7) = *(unsigned char *)(arg0 + 7) + 1;
            }
        }
        break;
    case 13:
        if (D_00810CB6[0] == 0) {
            if (D_00810CB0[0] != 0) {
                *(unsigned char *)(arg0 + 0x1F0) = 0x33;
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                *(unsigned char *)(arg0 + 7) = 0;
            } else {
                *(unsigned char *)(arg0 + 7) = 0;
            }
        } else if (D_00810CB6[0] == 1) {
            if (D_00810E74[0] & *(unsigned short *)0x70003B74) {
                D_00810CB6[0] = 2;
            }
        }
        break;
    }

    /* shared render tail */
    done = 0;
    if (*(unsigned char *)(arg0 + 0x1F0) != 0x33) {
        anim_matrix_dispatch(arg0);
        if (*(unsigned char *)(arg0 + 0x1F0) == 0x32 || *(unsigned char *)(arg0 + 0x1F0) == 0x35) {
            copy_qw4(arg0 + 0x2A0, D_00275B40[4] + 0x90);
            done = 1;
        } else if (*(unsigned char *)(arg0 + 0x275) == 4) {
            copy_qw4(arg0 + 0x2A0, D_00275B40[4] + 0x90);
            done = 1;
        } else if (*(unsigned char *)(arg0 + 0x2F2) != 0) {
            copy_qw4(arg0 + 0x2A0, D_00275B40[4] + 0x90);
            done = 1;
        }
        if (done == 0) {
            *(float *)(arg0 + 0x2D0) = *(float *)(D_00275B40[4] + 0xC0);
            *(float *)(arg0 + 0x2D4) = *(float *)(D_00275B40[4] + 0xC4);
            *(float *)(arg0 + 0x2D8) = *(float *)(D_00275B40[4] + 0xC8);
        }
    } else {
        anim_matrix_dispatch(arg0);
    }
}
