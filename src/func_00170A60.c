// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Entity sub-state machine on the byte at e+7 (cases 0,1,10,11,20,21,22,23,
// 30,31,32 with real fallthroughs 10->11, 20->21->22, 30->31), running under
// the mode byte at e+6/e+0x1F0. On entry (a1==0 and D_008106E0 set) it calls
// func_0017AF70. Every case falls into a common tail (duplicated inline in
// case 0's success path): unless mode==0x33 it runs anim_matrix_dispatch and
// either snapshots the camera quaternion (copy_qw4 from *(D_00275B40+0x10)
// +0x90 into e+0x2A0) when mode is 0x32/0x35, e[0x275]==4 or e[0x2F2] set, or
// copies the camera position floats +0xC0/C4/C8 into e+0x2D0/D4/D8. The
// 10/20/30 entries decrement the global pair D_00810C62/D_00810CB4, wake the
// owner entity (*(e+0x20)+0x2E=1) and fire clip 0x164/0x165 by mode; 11/22/31
// advance the timer at e+0x276 by 2 against float_to_int(e+0x2F4).
//
// MATCH NOTES: D_00810C62/D_00810CB4 must be declared volatile - the target
// serializes the two decrements (lbu/addiu/sb then lh/addiu/sh, v1 reused);
// non-volatile decls let the scheduler interleave them. The stop flag in
// cases 1/23/32 must be SEEDED from the call result (stop = func_001607D0(e))
// so it coalesces into v0, with the inner if/else putting stop=1 in the
// then-arm. Far globals as unknown-size arrays (D_00275B40 scalar for gp-rel).
// func_001607D0 declared unprototyped: cases 22/31 pass (e,1), others (e).
// mwcc 2.3.3 (mwcps2-2.3.3-000906) byte-matches at 100.0%; the pinned 991202
// build caps at 96.2%. Verified objdiff 100.0% vs build/expected.

extern void func_0017AF70(void);
extern int func_001607D0();
extern void anim_matrix_dispatch(unsigned char *e);
extern void copy_qw4(unsigned char *dst, unsigned char *src);
extern int func_0017B300(unsigned char *e, int f);
extern void func_001FBD50(unsigned char *e, int a, int b, float f);
extern void func_001FB9F0(int a, int b, int c, int d);
extern int float_to_int(float f);
extern int D_008106E0[];
extern unsigned char D_00810C61[];
extern volatile unsigned char D_00810C62[];
extern volatile short D_00810CB4[];
extern unsigned short D_00810E74[];
extern int D_00275B40;

void func_00170A60(unsigned char *e, int a1)
{
    unsigned char st;

    if (a1 == 0) {
        if (D_008106E0[0] != 0) {
            func_0017AF70();
        }
    }
    st = e[7];
    switch (st) {
    case 0:
        *(short *)(e + 0x2E) = 1;
        if (func_001607D0(e) != 0) {
            int done = 0;
            if (e[0x1F0] != 0x33) {
                anim_matrix_dispatch(e);
                if (e[0x1F0] == 0x32 || e[0x1F0] == 0x35) {
                    copy_qw4(e + 0x2A0, *(unsigned char **)(D_00275B40 + 0x10) + 0x90);
                    done = 1;
                } else if (e[0x275] == 4) {
                    copy_qw4(e + 0x2A0, *(unsigned char **)(D_00275B40 + 0x10) + 0x90);
                    done = 1;
                } else if (e[0x2F2] != 0) {
                    copy_qw4(e + 0x2A0, *(unsigned char **)(D_00275B40 + 0x10) + 0x90);
                    done = 1;
                }
                if (done == 0) {
                    *(float *)(e + 0x2D0) = *(float *)(*(unsigned char **)(D_00275B40 + 0x10) + 0xC0);
                    *(float *)(e + 0x2D4) = *(float *)(*(unsigned char **)(D_00275B40 + 0x10) + 0xC4);
                    *(float *)(e + 0x2D8) = *(float *)(*(unsigned char **)(D_00275B40 + 0x10) + 0xC8);
                }
            } else {
                anim_matrix_dispatch(e);
            }
            return;
        }
        e[0x2F2] = 1;
        if (e[0x274] != 0) {
            *(short *)(e + 0x2E) = 0;
            if (D_00810C62[0] != 0) {
                if (D_00810C61[0] == 0) {
                    e[7] = 0xA;
                } else if (D_00810C61[0] == 1) {
                    e[7] = 0x14;
                } else {
                    e[7] = 0x1E;
                }
            } else {
                e[0x274] = 0;
                func_001FBD50(e, 0x169, 0, 300.0f);
                if (D_00810C61[0] != 0) {
                    e[7] = e[7] + 1;
                }
            }
        } else if (D_00810E74[0] & 0x200) {
            if (func_0017B300(e, 2) == 0) {
                e[6] = e[6] + 1;
                e[7] = 0;
                e[0x1F0] = 0x33;
            }
        }
        break;
    case 1: {
        int stop = func_001607D0(e);
        if (stop == 0) {
            if (e[0x274] == 0) {
                stop = 1;
            } else {
                e[0x274] = 0;
                stop = 0;
            }
        } else {
            stop = 0;
        }
        if (stop != 0) {
            e[7] = 0;
        }
        break;
    }
    case 10:
        e[7] = st + 1;
        e[0x274] = 0;
        e[0x2F2] = 0;
        *(short *)(*(unsigned char **)(e + 0x20) + 0x2E) = 1;
        D_00810C62[0] = D_00810C62[0] - 1;
        D_00810CB4[0] = D_00810CB4[0] - 1;
        if (e[0x1F0] == 0x31 || e[0x1F0] == 0x34) {
            func_001FBD50(e, 0x164, 0, 300.0f);
        } else {
            func_001FBD50(e, 0x165, 0, 300.0f);
        }
        *(short *)(e + 0x2A) = 0;
        /* fallthrough */
    case 11:
        *(short *)(e + 0x276) = *(short *)(e + 0x276) + 2;
        if ((*(short *)(e + 0x276) >= float_to_int(*(float *)(e + 0x2F4)) - 8) &&
            (D_00810E74[0] & *(unsigned short *)0x70003B78)) {
            *(short *)(e + 0x2A) = 1;
        }
        if (*(short *)(e + 0x276) >= float_to_int(*(float *)(e + 0x2F4))) {
            *(short *)(e + 0x276) = 0;
            if (D_00810C62[0] == 0) {
                if (func_0017B300(e, 1) == 0) {
                    e[6] = e[6] + 1;
                    e[7] = 0;
                    e[0x1F0] = 0x33;
                } else {
                    e[7] = 0;
                }
            } else if (*(short *)(e + 0x2A) != 0) {
                e[0x2F2] = 1;
                e[0x274] = 1;
                e[7] = e[7] - 1;
            } else {
                e[7] = 0;
            }
        }
        break;
    case 20:
        e[7] = st + 1;
        *(short *)(e + 0x28) = 0;
        /* fallthrough */
    case 21:
        e[7] = e[7] + 1;
        e[0x274] = 0;
        if (*(short *)(e + 0x28) < 2) {
            *(float *)(e + 0x2F4) = 12.0f;
        }
        e[0x2F2] = 0;
        *(short *)(*(unsigned char **)(e + 0x20) + 0x2E) = 1;
        D_00810C62[0] = D_00810C62[0] - 1;
        D_00810CB4[0] = D_00810CB4[0] - 1;
        if (e[0x1F0] == 0x31 || e[0x1F0] == 0x34) {
            func_001FBD50(e, 0x164, 0, 300.0f);
        } else {
            func_001FBD50(e, 0x165, 0, 300.0f);
        }
        /* fallthrough */
    case 22:
        *(short *)(e + 0x276) = *(short *)(e + 0x276) + 2;
        if (*(short *)(e + 0x276) >= float_to_int(*(float *)(e + 0x2F4))) {
            *(short *)(e + 0x276) = 0;
            e[0x2F2] = 1;
            *(short *)(e + 0x28) = *(short *)(e + 0x28) + 1;
            if (D_00810C62[0] != 0) {
                if (*(short *)(e + 0x28) >= 3) {
                    e[7] = e[7] + 1;
                } else if (func_001607D0(e, 1) == 0) {
                    if (e[0x274] != 0) {
                        e[7] = e[7] - 1;
                    } else {
                        e[7] = 0;
                    }
                }
            } else {
                if (func_0017B300(e, 1) == 0) {
                    e[6] = e[6] + 1;
                    e[7] = 0;
                    e[0x1F0] = 0x33;
                } else {
                    if (*(short *)(e + 0x28) < 3) {
                        func_001FB9F0(0x169, 0x1000, 0x1000, 0x1000);
                    }
                    e[7] = e[7] + 1;
                }
            }
        }
        break;
    case 23: {
        int stop = func_001607D0(e);
        if (stop == 0) {
            if (e[0x274] == 0) {
                stop = 1;
            } else {
                e[0x274] = 0;
                stop = 0;
            }
        } else {
            stop = 0;
        }
        if (stop != 0) {
            e[7] = 0;
        }
        if (D_00810E74[0] & 0x200) {
            if (func_0017B300(e, 2) == 0) {
                e[6] = e[6] + 1;
                e[7] = 0;
                e[0x1F0] = 0x33;
                e[0x274] = 0;
            }
        }
        break;
    }
    case 30:
        e[7] = st + 1;
        e[0x274] = 0;
        *(float *)(e + 0x2F4) = 12.0f;
        e[0x2F2] = 0;
        *(short *)(*(unsigned char **)(e + 0x20) + 0x2E) = 1;
        D_00810C62[0] = D_00810C62[0] - 1;
        D_00810CB4[0] = D_00810CB4[0] - 1;
        if (e[0x1F0] == 0x31 || e[0x1F0] == 0x34) {
            func_001FBD50(e, 0x164, 0, 300.0f);
        } else {
            func_001FBD50(e, 0x165, 0, 300.0f);
        }
        /* fallthrough */
    case 31:
        *(short *)(e + 0x276) = *(short *)(e + 0x276) + 2;
        if (*(short *)(e + 0x276) >= float_to_int(*(float *)(e + 0x2F4))) {
            *(short *)(e + 0x276) = 0;
            e[0x2F2] = 1;
            if (D_00810C62[0] != 0) {
                if (func_001607D0(e, 1) == 0) {
                    if (e[0x274] != 0) {
                        e[7] = e[7] - 1;
                        e[0x274] = 0;
                    } else {
                        e[7] = 0;
                    }
                }
            } else {
                if (func_0017B300(e, 1) == 0) {
                    e[6] = e[6] + 1;
                    e[7] = 0;
                    e[0x1F0] = 0x33;
                } else {
                    func_001FB9F0(0x169, 0x1000, 0x1000, 0x1000);
                    e[7] = e[7] + 1;
                }
            }
        }
        break;
    case 32: {
        int stop = func_001607D0(e);
        if (stop == 0) {
            if (e[0x274] == 0) {
                stop = 1;
            } else {
                e[0x274] = 0;
                stop = 0;
            }
        } else {
            stop = 0;
        }
        if (stop != 0) {
            e[7] = 0;
        }
        break;
    }
    }

    {
    int done = 0;
    if (e[0x1F0] != 0x33) {
        anim_matrix_dispatch(e);
        if (e[0x1F0] == 0x32 || e[0x1F0] == 0x35) {
            copy_qw4(e + 0x2A0, *(unsigned char **)(D_00275B40 + 0x10) + 0x90);
            done = 1;
        } else if (e[0x275] == 4) {
            copy_qw4(e + 0x2A0, *(unsigned char **)(D_00275B40 + 0x10) + 0x90);
            done = 1;
        } else if (e[0x2F2] != 0) {
            copy_qw4(e + 0x2A0, *(unsigned char **)(D_00275B40 + 0x10) + 0x90);
            done = 1;
        }
        if (done == 0) {
            *(float *)(e + 0x2D0) = *(float *)(*(unsigned char **)(D_00275B40 + 0x10) + 0xC0);
            *(float *)(e + 0x2D4) = *(float *)(*(unsigned char **)(D_00275B40 + 0x10) + 0xC4);
            *(float *)(e + 0x2D8) = *(float *)(*(unsigned char **)(D_00275B40 + 0x10) + 0xC8);
        }
    } else {
        anim_matrix_dispatch(e);
    }
    }
}
