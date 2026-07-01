// NEARMISS func_00171670  (vram 0x00171670, 0x484 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.03% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Fully recovered logic/structure (entity fire-state-machine, sibling of matched func_00171B00/func_00171320 templates, correctly adds the sub-weapon-2-specific cases 2/12 using the no-prototype func_001607D0() no-arg overload idiom). Residual is register-allocation-ORDER: target reads the dispatch...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Entity anim-state-machine step (fire sub-machine, sub-weapon 2), dispatched
// on the mode byte at arg0+7. Sibling of func_00171320/func_00171B00 (same
// tail/matrix-pick block, duplicated verbatim by the compiler in both the
// case-1 fast-path and the shared tail).
// case 0: bump mode to 1, reset the frame index (arg0+0x28)=0; fall to case 1.
// case 1: mark clip index (arg0+0x2E)=1; if func_001607D0(arg0) is nonzero and
// the stage byte arg0+0x1F0 isn't already 0x33, run the matrix-pick block and
// return; else (0x1F0==0x33) call anim_matrix_dispatch and return. If
// func_001607D0(arg0)==0, set flag 0x2F2, and if arg0+0x274 is set, reset the
// clip via func_001FBD50(arg0,0x17A,0,300.0f) (when the live-instance counter
// D_00810CAA is 0) or advance mode to 10; either way fall through to the
// tail's matrix-pick block.
// case 2: if func_001607D0() (no-arg overload) is zero, treat as "picked" and
// possibly clear the pending-clip flag arg0+0x274 (if set, clear it and cancel
// picked); if picked, clear mode to 0; either way fall to the tail.
// case 10: reset flags, bump the linked entity's clip index, decrement the
// live-instance counter D_00810CAA, fire func_001FBD50 for a state-transition
// event (0x5DC for stage 0x31/0x34, 0x5DD otherwise), fall into case 11.
// case 11: advance the frame counter arg0+0x276; once it reaches
// float_to_int(arg0+0x2F4) frames, reset it and bump arg0+28 (frame-set
// index) and set flag 0x2F2; if instances remain (D_00810CAA != 0): once
// arg0+0x28 reaches 6, finish (stage=0x33, bump arg0+6, mode=0); else if
// func_001607D0(arg0)==0, mode = arg0+7 (unchanged) unless arg0+0x274 is set,
// in which case mode = arg0+7 - 1. If NO instances remain: below 6 frame-sets,
// re-fire func_001FBD50(arg0,0x17A,0,300.0f) and mode = arg0+7 + 1; otherwise
// mode = 1. Falls to the tail.
// case 12: same "picked" test as case 2 (func_001607D0() no-arg overload);
// mode forced to 1 either way (picked -> tail directly, else -> mode=1 tail).
// default (anything else): skip straight to the tail.
// tail / matrix-pick: unless stage byte arg0+0x1F0==0x33 (call
// anim_matrix_dispatch and return), call anim_matrix_dispatch then pick a
// matrix source: if stage byte is 0x32/0x35, or sub-state arg0+0x275==4, or
// flag arg0+0x2F2 set, copy_qw4 from *(D_00275B40+0x10)+0x90 into arg0+0x2A0;
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
    unsigned char mode;
    int done;
    unsigned char src;
    unsigned char mode2;

    mode = *(unsigned char *)(arg0 + 7);
    switch (mode) {
    case 0:
        *(unsigned char *)(arg0 + 7) = mode + 1;
        *(short *)(arg0 + 0x28) = 0;
        /* fallthrough */
    case 1:
        *(short *)(arg0 + 0x2E) = 1;
        if (func_001607D0((void *)arg0) != 0) {
            if (*(unsigned char *)(arg0 + 0x1F0) != 0x33) {
                anim_matrix_dispatch((void *)arg0);
                src = *(unsigned char *)(arg0 + 0x1F0);
                done = 0;
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
                return;
            }
            anim_matrix_dispatch((void *)arg0);
            return;
        }
        *(unsigned char *)(arg0 + 0x2F2) = 1;
        if (*(unsigned char *)(arg0 + 0x274) != 0) {
            *(short *)(arg0 + 0x2E) = 0;
            if (D_00810CAA[0] == 0) {
                *(unsigned char *)(arg0 + 0x274) = 0;
                func_001FBD50((void *)arg0, 0x17A, 0, 300.0f);
            } else {
                *(unsigned char *)(arg0 + 7) = 10;
            }
        }
        break;
    case 2:
        done = 0;
        if (func_001607D0() == 0) {
            done = 1;
            if (*(unsigned char *)(arg0 + 0x274) != 0) {
                *(unsigned char *)(arg0 + 0x274) = 0;
                done = 0;
            }
        }
        if (done != 0) {
            *(unsigned char *)(arg0 + 7) = 0;
        }
        break;
    case 10:
        *(unsigned char *)(arg0 + 7) = 11;
        *(unsigned char *)(arg0 + 0x274) = 0;
        *(unsigned char *)(arg0 + 0x2F2) = 0;
        *(short *)(*(int *)(arg0 + 0x20) + 0x2E) = 1;
        D_00810CAA[0] -= 1;
        src = *(unsigned char *)(arg0 + 0x1F0);
        if (src == 0x31 || src == 0x34) {
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
                } else if (func_001607D0((void *)arg0) == 0) {
                    mode2 = 1;
                    if (*(unsigned char *)(arg0 + 0x274) != 0) {
                        mode2 = *(unsigned char *)(arg0 + 7) - 1;
                    }
                    *(unsigned char *)(arg0 + 7) = mode2;
                }
            } else {
                if (*(short *)(arg0 + 0x28) < 6) {
                    func_001FBD50((void *)arg0, 0x17A, 0, 300.0f);
                    mode2 = *(unsigned char *)(arg0 + 7) + 1;
                    *(unsigned char *)(arg0 + 7) = mode2;
                } else {
                    *(unsigned char *)(arg0 + 7) = 1;
                }
            }
        }
        break;
    case 12:
        done = 0;
        if (func_001607D0() == 0) {
            done = 1;
            if (*(unsigned char *)(arg0 + 0x274) != 0) {
                *(unsigned char *)(arg0 + 0x274) = 0;
                done = 0;
            }
        }
        if (done == 0) {
            *(unsigned char *)(arg0 + 7) = 1;
        }
        break;
    default:
        break;
    }

    if (*(unsigned char *)(arg0 + 0x1F0) != 0x33) {
        anim_matrix_dispatch((void *)arg0);
        src = *(unsigned char *)(arg0 + 0x1F0);
        done = 0;
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
        return;
    }
    anim_matrix_dispatch((void *)arg0);
}
