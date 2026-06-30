// NEARMISS func_00191D40  (vram 0x00191D40, 0x2C4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.27% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 91.27% near-miss, logic fully recovered and FP register coloring (fparg1=f21, delta=f20, fparg0 reused in f12) matches. Sole residual: both 'if(fparg1<=step) step=fparg1;' min ops are emitted by the 2.3.1 target as branch-likely 'bc1tl' + mov.s in delay slot PLUS a dead duplicate mov.s after the ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 91.27% (logic fully recovered, FP register coloring correct).
// Camera/height follow: target = clamp(arg0+0x98 + in0, <= cap 0x54),
// delta = target - cur(0x14); steps cur toward target. While rising
// (delta>0,!flag0x80) or falling (delta<=0,!flag0x40 && !(0x5A&1)) it moves
// cur by min(|delta|/10, in1) when |delta|>1 else delta/5, then a per-mode
// (D_00810700/01/02) post-clamp bounds cur. fparg0 mutated in place so it
// reuses the f12 arg register as the target does.
// RESIDUAL WALL: both float-min ops (if(fparg1<=step) step=fparg1;) are
// emitted by the 2.3.1 target as branch-LIKELY 'bc1tl' with 'mov.s f1,f21'
// in the delay slot PLUS a DEAD duplicate 'mov.s f1,f21' after the
// fall-through branch (mwcc 2.3.1 branch-likely dead-instr codegen). mwcc
// 2.3.3 and 991202 both emit plain 'bc1f'+nop and never the dead-duplicate
// form, so this 2.3.1-specific lowering is unreachable from plain C; the
// whole tail cascades from these two single-instruction deltas.
extern float func_0011DF78(float);
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;

void func_00191D40(char *arg0, float fparg0, float fparg1) {
    float lim;
    float delta;
    float ab;
    float step;
    float cur;

    lim = *(float *)(arg0 + 0x54);
    fparg0 = fparg0 + *(float *)(arg0 + 0x98);
    if (!(fparg0 <= lim)) {
        fparg0 = lim;
    }
    delta = fparg0 - *(float *)(arg0 + 0x14);
    ab = func_0011DF78(delta);
    if (!(delta <= 0.0f)) {
        if (!(*(unsigned char *)(arg0 + 7) & 0x80)) {
            if (!(ab <= 1.0f)) {
                step = ab / 10.0f;
                if (fparg1 <= step) {
                    step = fparg1;
                }
                cur = *(float *)(arg0 + 0x14) + step;
            } else {
                cur = *(float *)(arg0 + 0x14) + delta / 5.0f;
            }
            *(float *)(arg0 + 0x14) = cur;
        }
    } else if (!(*(unsigned char *)(arg0 + 7) & 0x40) && !(*(short *)(arg0 + 0x5A) & 1)) {
        if (!(ab <= 1.0f)) {
            step = ab / 10.0f;
            if (fparg1 <= step) {
                step = fparg1;
            }
            cur = *(float *)(arg0 + 0x14) - step;
        } else {
            cur = *(float *)(arg0 + 0x14) + delta / 5.0f;
        }
        *(float *)(arg0 + 0x14) = cur;
    }
    if (D_00810700 == 0x10) {
        if (D_00810701 == 1) {
            if (D_00810702 == 2 || D_00810702 == 4 || D_00810702 == 6) {
                float pos = *(float *)(arg0 + 0x50);
                if (!(pos <= 100.0f)) {
                    float top = 7.5f + pos;
                    if (!(*(float *)(arg0 + 0x14) <= top)) {
                        *(float *)(arg0 + 0x14) = top;
                    }
                }
            }
        }
    } else if (D_00810700 == 3 && D_00810701 == 1) {
        float h = *(float *)(arg0 + 0x14);
        if (!(h <= 250.0f) && *(float *)(arg0 + 0x10) < 356.0f) {
            float lo = *(float *)(arg0 + 0x54) - 2.0f;
            if (!(h <= lo)) {
                *(float *)(arg0 + 0x14) = lo;
            }
        }
    }
}
