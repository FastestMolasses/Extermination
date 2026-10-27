// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 91.03% NEARMISS).
// Fixes read from the original instructions (spelling follows the byte-matched
// sibling func_00171320):
//  - func_001607D0 takes the entity in cases 1 and 11 (its definition in src/
//    has one parameter); cases 2 and 12 call it with no argument set up, kept
//    through the unprototyped declaration.
//  - The matrix-pick block clears its "copied" flag first and is an if/else on
//    the 0x33 stage byte, not an early return (both copies).
//  - Case 1, no live instances: the mode byte is also incremented (1 -> 2)
//    before the clip reset; the old C dropped that store.
//  - Case 10 writes mode+1; the 0x31/0x34 test is one OR condition.
//  - Cases 2/12 use their own "picked" flag (not the matrix-pick flag), set
//    only after the call, as an if/else; case 12 sets mode=1 when picked
//    (the old C had the condition inverted).
//  - Case 11 writes the mode byte directly in each arm.
//
// Entity anim-state-machine step (fire sub-machine, sub-weapon 2), dispatched
// on the mode byte at arg0+7. Sibling of func_00171320/func_00171B00.
// case 0: bump mode to 1, reset the frame-set index (arg0+0x28)=0; fall to case 1.
// case 1: mark clip index (arg0+0x2E)=1; if func_001607D0(arg0) is nonzero, run
// the matrix-pick block and return. Otherwise set flag 0x2F2, and if arg0+0x274
// is set: clear the clip index, then either (no live instances, D_00810CAA==0)
// advance mode by one, clear 0x274 and reset the clip via
// func_001FBD50(arg0,0x17A,0,300.0f), or set mode to 10; fall to the tail.
// case 2: "picked" = func_001607D0() returned zero and 0x274 was clear (a set
// 0x274 is consumed: cleared, not picked); if picked, mode = 0. Falls to the tail.
// case 10: advance mode to 11, reset flags, bump the linked entity's clip index,
// decrement D_00810CAA, fire func_001FBD50(arg0, 0x5DC for stage 0x31/0x34 else
// 0x5DD, 0, 300.0f), fall into case 11.
// case 11: advance the frame counter arg0+0x276; once it reaches
// float_to_int(arg0+0x2F4) frames, reset it, bump the frame-set index arg0+0x28
// and set flag 0x2F2. If instances remain: at 6 frame-sets finish (stage=0x33,
// bump arg0+6, mode=0); else if func_001607D0(arg0)==0, mode = mode-1 when
// 0x274 is set, otherwise 1. If none remain: below 6 frame-sets re-fire
// func_001FBD50(arg0,0x17A,0,300.0f) and mode+=1; otherwise mode = 1.
// case 12: same "picked" test as case 2; if picked, mode = 1.
// default: skip straight to the tail.
// matrix-pick block (emitted twice: case-1 fast path and the shared tail): if
// stage byte arg0+0x1F0 is 0x33 just call anim_matrix_dispatch; else call it and
// pick a matrix source: if stage byte is 0x32/0x35, or sub-state arg0+0x275==4,
// or flag arg0+0x2F2 set, copy_qw4 from *(D_00275B40+0x10)+0x90 into arg0+0x2A0;
// else copy the base translate (*(D_00275B40+0x10)+0xC0/C4/C8) into
// arg0+0x2D0/2D4/2D8.
extern void anim_matrix_dispatch(void *arg0);
extern void copy_qw4(void *dst, void *src);
extern int float_to_int(float f);
extern int func_001607D0();
extern void func_001FBD50(void *p, int a, int b, float f);
extern int D_00275B40;
extern short D_00810CAA[8];

void func_00171670(int arg0) {
    int done;
    unsigned char mode;
    unsigned char src;
    int ok;

    mode = *(unsigned char *)(arg0 + 7);
    switch (mode) {
    case 0:
        *(unsigned char *)(arg0 + 7) = mode + 1;
        *(short *)(arg0 + 0x28) = 0;
        /* fallthrough */
    case 1:
        *(short *)(arg0 + 0x2E) = 1;
        if (func_001607D0((char *)arg0) != 0) {
            done = 0;
            if (*(unsigned char *)(arg0 + 0x1F0) != 0x33) {
                anim_matrix_dispatch((void *)arg0);
                src = *(unsigned char *)(arg0 + 0x1F0);
                if (src == 0x32 || src == 0x35) {
                    copy_qw4((void *)(arg0 + 0x2A0), (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
                    done = 1;
                } else if (*(unsigned char *)(arg0 + 0x275) == 4) {
                    copy_qw4((void *)(arg0 + 0x2A0), (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
                    done = 1;
                } else if (*(unsigned char *)(arg0 + 0x2F2) != 0) {
                    copy_qw4((void *)(arg0 + 0x2A0), (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
                    done = 1;
                }
                if (done == 0) {
                    *(float *)(arg0 + 0x2D0) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC0);
                    *(float *)(arg0 + 0x2D4) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC4);
                    *(float *)(arg0 + 0x2D8) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC8);
                }
            } else {
                anim_matrix_dispatch((void *)arg0);
            }
            return;
        }
        *(unsigned char *)(arg0 + 0x2F2) = 1;
        if (*(unsigned char *)(arg0 + 0x274) != 0) {
            *(short *)(arg0 + 0x2E) = 0;
            if (D_00810CAA[0] == 0) {
                *(unsigned char *)(arg0 + 7) = *(unsigned char *)(arg0 + 7) + 1;
                *(unsigned char *)(arg0 + 0x274) = 0;
                func_001FBD50((void *)arg0, 0x17A, 0, 300.0f);
            } else {
                *(unsigned char *)(arg0 + 7) = 10;
            }
        }
        break;
    case 2:
        if (func_001607D0() == 0) {
            if (*(unsigned char *)(arg0 + 0x274) == 0) {
                ok = 1;
            } else {
                *(unsigned char *)(arg0 + 0x274) = 0;
                ok = 0;
            }
        } else {
            ok = 0;
        }
        if (ok != 0) {
            *(unsigned char *)(arg0 + 7) = 0;
        }
        break;
    case 10:
        *(unsigned char *)(arg0 + 7) = mode + 1;
        *(unsigned char *)(arg0 + 0x274) = 0;
        *(unsigned char *)(arg0 + 0x2F2) = 0;
        *(short *)(*(int *)(arg0 + 0x20) + 0x2E) = 1;
        D_00810CAA[0] -= 1;
        if (*(unsigned char *)(arg0 + 0x1F0) == 0x31 || *(unsigned char *)(arg0 + 0x1F0) == 0x34) {
            func_001FBD50((void *)arg0, 0x5DC, 0, 300.0f);
        } else {
            func_001FBD50((void *)arg0, 0x5DD, 0, 300.0f);
        }
        /* fallthrough */
    case 11:
        *(short *)(arg0 + 0x276) = (short)(*(short *)(arg0 + 0x276) + 1);
        if (*(short *)(arg0 + 0x276) >= float_to_int(*(float *)(arg0 + 0x2F4))) {
            *(short *)(arg0 + 0x276) = 0;
            *(short *)(arg0 + 0x28) = (short)(*(short *)(arg0 + 0x28) + 1);
            *(unsigned char *)(arg0 + 0x2F2) = 1;
            if (D_00810CAA[0] != 0) {
                if (*(short *)(arg0 + 0x28) >= 6) {
                    *(unsigned char *)(arg0 + 0x1F0) = 0x33;
                    *(unsigned char *)(arg0 + 6) = (unsigned char)(*(unsigned char *)(arg0 + 6) + 1);
                    *(unsigned char *)(arg0 + 7) = 0;
                } else if (func_001607D0((char *)arg0) == 0) {
                    if (*(unsigned char *)(arg0 + 0x274) != 0) {
                        *(unsigned char *)(arg0 + 7) = *(unsigned char *)(arg0 + 7) - 1;
                    } else {
                        *(unsigned char *)(arg0 + 7) = 1;
                    }
                }
            } else {
                if (*(short *)(arg0 + 0x28) < 6) {
                    func_001FBD50((void *)arg0, 0x17A, 0, 300.0f);
                    *(unsigned char *)(arg0 + 7) = *(unsigned char *)(arg0 + 7) + 1;
                } else {
                    *(unsigned char *)(arg0 + 7) = 1;
                }
            }
        }
        break;
    case 12:
        if (func_001607D0() == 0) {
            if (*(unsigned char *)(arg0 + 0x274) == 0) {
                ok = 1;
            } else {
                *(unsigned char *)(arg0 + 0x274) = 0;
                ok = 0;
            }
        } else {
            ok = 0;
        }
        if (ok != 0) {
            *(unsigned char *)(arg0 + 7) = 1;
        }
        break;
    default:
        break;
    }

    done = 0;
    if (*(unsigned char *)(arg0 + 0x1F0) != 0x33) {
        anim_matrix_dispatch((void *)arg0);
        src = *(unsigned char *)(arg0 + 0x1F0);
        if (src == 0x32 || src == 0x35) {
            copy_qw4((void *)(arg0 + 0x2A0), (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
            done = 1;
        } else if (*(unsigned char *)(arg0 + 0x275) == 4) {
            copy_qw4((void *)(arg0 + 0x2A0), (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
            done = 1;
        } else if (*(unsigned char *)(arg0 + 0x2F2) != 0) {
            copy_qw4((void *)(arg0 + 0x2A0), (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
            done = 1;
        }
        if (done == 0) {
            *(float *)(arg0 + 0x2D0) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC0);
            *(float *)(arg0 + 0x2D4) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC4);
            *(float *)(arg0 + 0x2D8) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC8);
        }
    } else {
        anim_matrix_dispatch((void *)arg0);
    }
}
