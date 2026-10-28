// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 91.27% NEARMISS).
// Fixes read from the original instructions:
// - The two float mins are ternaries `step = (fparg1 > step) ? step : fparg1;`
//   (the byte-matched twin func_00192010's spelling), not if-assignments; that
//   produces the original branch-likely min with its duplicated move. The
//   "2.3.1-only codegen" verdict was wrong.
// - The steps update the height in place (`+=` / `-=`), no temporary.
// - The post-clamp is `if (mode == 0x10 && sub == 1) ... else if (mode == 3 &&
//   sub == 1)`, so a 0x10 mode with sub != 1 falls through to the mode-3 test.
// - The 0x10 clamp compares against `7.5f + pos` written inline (twice), no temp.
//
// Camera/height follow: target = clamp(arg0+0x98 + in0, <= cap 0x54),
// delta = target - cur(0x14); steps cur toward target. While rising
// (delta>0,!flag0x80) or falling (delta<=0,!flag0x40 && !(0x5A&1)) it moves
// cur by min(|delta|/10, in1) when |delta|>1 else delta/5, then a per-mode
// (D_00810700/01/02) post-clamp bounds cur. fparg0 is updated in place.
extern float func_0011DF78(float);
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;

void func_00191D40(char *arg0, float fparg0, float fparg1) {
    float lim;
    float delta;
    float ab;
    float step;

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
                step = (fparg1 > step) ? step : fparg1;
                *(float *)(arg0 + 0x14) += step;
            } else {
                *(float *)(arg0 + 0x14) = *(float *)(arg0 + 0x14) + delta / 5.0f;
            }
        }
    } else if (!(*(unsigned char *)(arg0 + 7) & 0x40) && !(*(short *)(arg0 + 0x5A) & 1)) {
        if (!(ab <= 1.0f)) {
            step = ab / 10.0f;
            step = (fparg1 > step) ? step : fparg1;
            *(float *)(arg0 + 0x14) -= step;
        } else {
            *(float *)(arg0 + 0x14) = *(float *)(arg0 + 0x14) + delta / 5.0f;
        }
    }
    if (D_00810700 == 0x10 && D_00810701 == 1) {
        if (D_00810702 == 2 || D_00810702 == 4 || D_00810702 == 6) {
            float pos = *(float *)(arg0 + 0x50);
            if (!(pos <= 100.0f)) {
                if (!(*(float *)(arg0 + 0x14) <= 7.5f + pos)) {
                    *(float *)(arg0 + 0x14) = 7.5f + pos;
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
