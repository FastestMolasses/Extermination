// NEARMISS func_00171E90  (vram 0x00171E90, 0x53C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.56% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Same documented saved-register-allocation-order class as sibling func_00171B00.c (already parked at 92.31%): the self pointer and the tail matrix-pick block's done flag get s1/s0 in the target but s0/s1 (and a stray extra reg) in mwcc -- a global, consistent register-pair swap across ~100 instruc...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Entity anim-state-machine step, dispatched on the mode byte at arg0+7.
// Sibling of func_00171B00 (same tail/matrix-pick block, duplicated verbatim
// by the compiler in both the case-1 fast-path and the shared tail), with
// extra states 2/12 (a "wait for instance slot" retry state and a periodic
// clip-advance state driven by a live-instance counter D_00810CAE).
// case 0: bump mode to 1; if D_00810CAE==0, poll func_0017B420(); fall to case 1.
// case 1: mark clip index (arg0+0x2E)=1; if func_001607D0(arg0) is nonzero and
// the stage byte arg0+0x1F0 isn't already 0x33, run the matrix-pick block and
// return; else (0x1F0==0x33) call anim_matrix_dispatch and return. If
// func_001607D0(arg0)==0, set flag 0x2F2; if arg0+0x274 is set, clear the clip
// index and either fire func_001FBD50(arg0,0x17A,0,300.0f) (D_00810CAE==0) or
// advance mode to 10 (else). Falls to the tail.
// case 2: retry state -- if func_001607D0(arg0)==0, and arg0+0x274 is set,
// clear it and fall to the tail; else reset mode to 0 and skip the tail.
// case 10: reset flags, bump the linked entity's clip index, decrement
// D_00810CAE, fire func_001FBD50 for a state-transition event (0x5DC), fall
// into case 11.
// case 11: advance the frame counter arg0+0x28; once it hits 0, and instances
// remain (D_00810CAE==0), poll func_0017B420() to advance the stage byte and
// bump arg0+6, or else start a fresh async play (func_001FBD50 id 0x5DD) and
// track it via arg0+0x31A/0x31B/0x31C. Falls to the tail.
// case 12: advance-on-third-tick state -- if func_001607D0(arg0)==0 and
// arg0+0x274 set, clear it, and every 3rd tick of arg0+0x28 resets the linked
// entity's clip index; once arg0+0x28 reaches 6, wrap it and decrement
// D_00810CAE, driving the same func_0017B420 poll as case 11. Also manages an
// async play tracked via arg0+0x31A/0x31B/0x31C (stops it via func_0011A070
// once mode leaves 12, restarts it if it lapsed). Falls to the tail.
// default: skip straight to the tail.
// tail / matrix-pick: unless stage byte arg0+0x1F0==0x33 (call
// anim_matrix_dispatch and return), call anim_matrix_dispatch then pick a
// matrix source: if stage byte is 0x32/0x35, or sub-state arg0+0x275==4, or
// flag arg0+0x2F2 set, copy_qw4 from *(D_00275B40+0x10)+0x90 into arg0+0x2A0;
// else copy the base translate (*(D_00275B40+0x10)+0xC0/C4/C8) into
// arg0+0x2D0/2D4/2D8.
extern void anim_matrix_dispatch(void *arg0);
extern void copy_qw4(void *dst, void *src);
extern void func_0011A070(int id);
extern int func_001607D0(void *arg0);
extern int func_0017B420(void);
extern int func_001FBD50(void *p, int a, int b, float f);
extern int D_00275B40;
extern short D_00810CAE[8];

void func_00171E90(int arg0) {
    unsigned char mode;
    unsigned char src;
    int done;

    mode = *(unsigned char *)(arg0 + 7);
    switch (mode) {
    case 0:
        *(unsigned char *)(arg0 + 7) = mode + 1;
        if (D_00810CAE[0] == 0) {
            func_0017B420();
        }
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
            if (D_00810CAE[0] == 0) {
                *(unsigned char *)(arg0 + 7) = (unsigned char)(*(unsigned char *)(arg0 + 7) + 1);
                *(unsigned char *)(arg0 + 0x274) = 0;
                func_001FBD50((void *)arg0, 0x17A, 0, 300.0f);
            } else {
                *(unsigned char *)(arg0 + 7) = 0xA;
            }
        }
        break;
    case 2:
        done = 0;
        if (func_001607D0((void *)arg0) == 0) {
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
        *(unsigned char *)(arg0 + 7) = mode + 1;
        *(short *)(arg0 + 0x28) = 4;
        *(unsigned char *)(arg0 + 0x274) = 0;
        *(short *)(*(int *)(arg0 + 0x20) + 0x2E) = 1;
        D_00810CAE[0] -= 1;
        *(unsigned char *)(arg0 + 0x2F2) = 0;
        func_001FBD50((void *)arg0, 0x5DC, 0, 300.0f);
        *(char *)(arg0 + 0x2F1) = 0xFF;
        break;
    case 11: {
        short f28 = *(short *)(arg0 + 0x28);
        *(short *)(arg0 + 0x28) = f28 - 1;
        if (f28 == 0) {
            if (D_00810CAE[0] == 0) {
                if (func_0017B420() == 0) {
                    *(unsigned char *)(arg0 + 6) = (unsigned char)(*(unsigned char *)(arg0 + 6) + 1);
                    *(unsigned char *)(arg0 + 7) = 0;
                    *(unsigned char *)(arg0 + 0x1F0) = 0x33;
                } else {
                    *(unsigned char *)(arg0 + 7) = 0;
                }
                *(short *)(*(int *)(arg0 + 0x20) + 0x2E) = 2;
            } else {
                *(unsigned char *)(arg0 + 7) = (unsigned char)(*(unsigned char *)(arg0 + 7) + 1);
                *(char *)(arg0 + 0x31B) = (char)func_001FBD50((void *)arg0, 0x5DD, 0, 300.0f);
                *(unsigned char *)(arg0 + 0x31A) = 1;
                *(unsigned short *)(arg0 + 0x31C) = 0x5DD;
                *(short *)(arg0 + 0x28) = 0;
            }
        }
        break;
    }
    case 12:
        if (func_001607D0((void *)arg0) == 0) {
            if (*(unsigned char *)(arg0 + 0x274) != 0) {
                *(unsigned char *)(arg0 + 0x274) = 0;
                if ((*(short *)(arg0 + 0x28) % 3) == 0) {
                    *(short *)(*(int *)(arg0 + 0x20) + 0x2E) = 1;
                }
                *(short *)(arg0 + 0x28) = (short)(*(short *)(arg0 + 0x28) + 1);
                if (*(short *)(arg0 + 0x28) >= 6) {
                    *(short *)(arg0 + 0x28) = 0;
                    D_00810CAE[0] -= 1;
                    if (D_00810CAE[0] == 0) {
                        if (func_0017B420() == 0) {
                            *(unsigned char *)(arg0 + 6) = (unsigned char)(*(unsigned char *)(arg0 + 6) + 1);
                            *(unsigned char *)(arg0 + 7) = 0;
                            *(unsigned char *)(arg0 + 0x1F0) = 0x33;
                        } else {
                            *(unsigned char *)(arg0 + 7) = 0;
                        }
                        *(short *)(*(int *)(arg0 + 0x20) + 0x2E) = 2;
                    }
                }
            } else {
                *(unsigned char *)(arg0 + 7) = 0;
            }
        }
        if (*(unsigned char *)(arg0 + 7) != 0xC) {
            char slot = *(char *)(arg0 + 0x31B);
            if (slot != -1) {
                func_0011A070(slot);
                *(char *)(arg0 + 0x31B) = -1;
                *(unsigned char *)(arg0 + 0x31A) = 0;
            }
        } else if (*(unsigned char *)(arg0 + 0x31A) != 0 && *(unsigned short *)(arg0 + 0x31C) == 0x5DD &&
                   *(char *)(arg0 + 0x31B) == -1) {
            *(char *)(arg0 + 0x31B) = (char)func_001FBD50((void *)arg0, 0x5DD, 0, 300.0f);
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
