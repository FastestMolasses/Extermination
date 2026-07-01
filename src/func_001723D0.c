// NEARMISS func_001723D0  (vram 0x001723D0, 0x48C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.77% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register/scheduling-artifact noise in the twice-duplicated animdispatch+copy_qw4 tail block: (a) saved-reg allocation-order swap (target keeps arg0 in s1 and a done-style local in s0; ours keeps arg0 in s0 with no separate local surviving to s1 -- same class as func_00135870's wall), and (b) targ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS: entity state machine (states 0/10-13; anim-turn-and-render dispatch).
// Logic/body fully recovered incl. the "func_001607D0()==0" early-out (arg0+0x2E
// gets the CONSTANT 1 materialized into the jal delay slot, then the real branch
// tests the call's v0 result -- NOT the stored value). Residual is register/
// scheduling-artifact noise in the twice-duplicated animdispatch+copy_qw4 tail
// block: (a) a genuine saved-reg allocation-order swap (target: arg0->s1, a
// `done`-style local->s0; ours: arg0->s0, no separate local survives to s1 --
// same class as func_00135870/func_00151E50's reg-order wall), and (b) target
// reloads *(arg0+0x1F0) fresh for the second half of the `kind==0x32||kind==0x35`
// OR-test (idiom-15 reload-across-nothing) and keeps `done` materialized in a
// saved reg from the very first branch's delay slot, whereas our compile CSEs
// the read and lets `done` live in a scratch/arg reg with an early `b` after the
// first arm -- no source-level lever found that flips this without regressing
// elsewhere; mwcc233 measured 86.77%, mwcc(991202) 84.81% (worse).
extern void anim_matrix_dispatch(char *self);
extern void copy_qw4(char *dst, char *src);
extern short float_to_int(float);
extern int func_001607D0(void);
extern void func_001FBD50(char *p, int a, int b, float f);
extern char **D_00275B40;
extern short D_00810CB0;
extern unsigned char D_00810CB6;
extern unsigned short D_00810E74;

void func_001723D0(char *arg0) {
    unsigned char st;
    int done;
    char *m;

    st = *(unsigned char *)(arg0 + 7);
    switch (st) {
    case 0:
        if (D_00810CB6 == 1) {
            if (D_00810E74 & *(unsigned short *)0x70003B74) {
                D_00810CB6 = 2;
            }
            break;
        }
        if (D_00810CB6 != 0) {
            break;
        }
        *(short *)(arg0 + 0x2E) = 1;
        if (func_001607D0() != 0) {
            if (*(unsigned char *)(arg0 + 0x1F0) == 0x33) {
                anim_matrix_dispatch(arg0);
                return;
            }
            anim_matrix_dispatch(arg0);
            done = 0;
            if ((*(unsigned char *)(arg0 + 0x1F0) == 0x32) ||
                (*(unsigned char *)(arg0 + 0x1F0) == 0x35)) {
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
                m = D_00275B40[4];
                *(float *)(arg0 + 0x2D0) = *(float *)(m + 0xC0);
                *(float *)(arg0 + 0x2D4) = *(float *)(m + 0xC4);
                *(float *)(arg0 + 0x2D8) = *(float *)(m + 0xC8);
            }
            return;
        }
        *(unsigned char *)(arg0 + 0x2F2) = 1;
        if (*(unsigned char *)(arg0 + 0x274) != 0) {
            *(short *)(arg0 + 0x2E) = 0;
            if (D_00810CB0 == 0) {
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
            D_00810CB0 = D_00810CB0 - 1;
            if (*(unsigned char *)(arg0 + 0x1F0) == 0x31 || *(unsigned char *)(arg0 + 0x1F0) == 0x34) {
                func_001FBD50(arg0, 0x5DC, 0, 300.0f);
            } else {
                func_001FBD50(arg0, 0x5DD, 0, 300.0f);
            }
            D_00810CB6 = 1;
        }
        break;
    case 12:
        if (D_00810CB6 == 1 && (D_00810E74 & *(unsigned short *)0x70003B74)) {
            D_00810CB6 = 2;
        }
        *(short *)(arg0 + 0x276) = *(short *)(arg0 + 0x276) + 1;
        if (*(short *)(arg0 + 0x276) >= float_to_int(*(float *)(arg0 + 0x2F4))) {
            *(short *)(arg0 + 0x276) = 0;
            *(unsigned char *)(arg0 + 0x2F2) = 1;
            if (D_00810CB6 == 0) {
                if (D_00810CB0 != 0) {
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
        if (D_00810CB6 == 0) {
            if (D_00810CB0 != 0) {
                *(unsigned char *)(arg0 + 0x1F0) = 0x33;
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                *(unsigned char *)(arg0 + 7) = 0;
            } else {
                *(unsigned char *)(arg0 + 7) = 0;
            }
        } else if (D_00810CB6 == 1) {
            if (D_00810E74 & *(unsigned short *)0x70003B74) {
                D_00810CB6 = 2;
            }
        }
        break;
    }

    /* shared render tail */
    if (*(unsigned char *)(arg0 + 0x1F0) == 0x33) {
        anim_matrix_dispatch(arg0);
        return;
    }
    anim_matrix_dispatch(arg0);
    done = 0;
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
        m = D_00275B40[4];
        *(float *)(arg0 + 0x2D0) = *(float *)(m + 0xC0);
        *(float *)(arg0 + 0x2D4) = *(float *)(m + 0xC4);
        *(float *)(arg0 + 0x2D8) = *(float *)(m + 0xC8);
    }
}
