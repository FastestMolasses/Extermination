// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS: leaf blend-weight driver for entity arg0, dispatched on the mode byte
// at arg0+0x1F0 (jump table jtbl_0026D720, 8 entries; modes 0/3/4/5/7 do nothing).
// arg0+0x25C is a step index into the parallel keyframe-threshold tables
// D_0024886C / D_00248870 / D_00248874 (lower / current / upper bound) and the
// per-frame delta tables D_00248880 (rise) / D_00248890 (fall). arg0+0x38 is the
// running scalar; arg0+0x204 / arg0+0x208 are the published blend weights, staged
// through the scratchpad word at 0x70003A20.
//   mode 1: sub-mode byte at arg0+0x1F1 selects rise (1), fall (2) or settle (other).
//           Rise advances arg0+0x38 by D_00248880[i]; on reaching D_00248874[i] it
//           clamps, bumps the step index and clears the sub-mode, otherwise it
//           publishes the interpolated weights. Fall is the mirror image using
//           D_00248890[i] / D_0024886C[i] and decrements the step index. Settle
//           compares arg0+0x38 against the target arg0+0x240 and re-arms the
//           sub-mode (below -> 1, above -> 2/mode 3, equal at step 2 -> weight 0.75).
//   mode 2: decays arg0+0x38 by 0.03125 (doubled when any of the low five bits of
//           arg0+0x314 is set); on crossing D_0024886C[i] it clamps and either
//           re-arms mode 1 (if arg0+0x23F is set) or resets to mode 3, then always
//           republishes the weights with a 0.5 gain.
//   mode 6: bleeds arg0+0x38 down by 0.05 per frame, floored at 0.
extern float D_0024886C[];
extern float D_00248870[];
extern float D_00248874[];
extern float D_00248880[];
extern float D_00248890[];

void func_0017BC40(char *arg0) {
    int i;
    float x;
    float hi;
    float lo;
    float step;
    float zero;

    switch (*(unsigned char *)(arg0 + 0x1F0)) {
    case 0:
    case 3:
    case 4:
    case 5:
    case 7:
        break;
    case 1:
        if (*(unsigned char *)(arg0 + 0x1F1) == 1) {
            i = *(unsigned char *)(arg0 + 0x25C);
            x = *(float *)(arg0 + 0x38) + D_00248880[i];
            lo = D_00248870[i];
            hi = D_00248874[i];
            *(float *)(arg0 + 0x38) = x;
            if (x >= hi) {
                *(float *)(arg0 + 0x38) = hi;
                *(unsigned char *)(arg0 + 0x25C) = *(unsigned char *)(arg0 + 0x25C) + 1;
                *(unsigned char *)(arg0 + 0x1F1) = 0;
            } else {
                *(float *)0x70003A20 = 1.0f - (hi - x) / (hi - lo);
                *(float *)(arg0 + 0x208) = *(float *)0x70003A20;
                *(float *)(arg0 + 0x204) = 1.0f + *(float *)0x70003A20;
            }
        } else if (*(unsigned char *)(arg0 + 0x1F1) == 2) {
            zero = 0.0f;
            if (*(float *)(arg0 + 0x240) == zero) {
                if (*(unsigned char *)(arg0 + 0x25C) < 3) {
                    *(unsigned char *)(arg0 + 0x1F0) = 3;
                } else {
                    *(unsigned char *)(arg0 + 0x1F0) = 2;
                    *(int *)(arg0 + 0x208) = 0;
                }
            } else {
                i = *(unsigned char *)(arg0 + 0x25C);
                x = *(float *)(arg0 + 0x38) - D_00248890[i];
                lo = D_00248870[i];
                hi = D_0024886C[i];
                *(float *)(arg0 + 0x38) = x;
                if (x <= hi) {
                    *(float *)(arg0 + 0x38) = hi;
                    *(unsigned char *)(arg0 + 0x25C) = *(unsigned char *)(arg0 + 0x25C) - 1;
                    *(unsigned char *)(arg0 + 0x1F1) = 0;
                } else {
                    *(float *)0x70003A20 = 1.0f - (hi - x) / (hi - lo);
                    *(float *)(arg0 + 0x208) = *(float *)0x70003A20;
                    *(float *)(arg0 + 0x204) = 1.0f + *(float *)0x70003A20;
                }
            }
        } else {
            zero = 0.0f;
            if (*(float *)(arg0 + 0x240) == zero) {
                if (*(unsigned char *)(arg0 + 0x25C) < 3) {
                    *(unsigned char *)(arg0 + 0x1F0) = 3;
                } else {
                    *(unsigned char *)(arg0 + 0x1F0) = 2;
                    *(int *)(arg0 + 0x208) = 0;
                }
            } else if (*(float *)(arg0 + 0x38) < *(float *)(arg0 + 0x240)) {
                *(unsigned char *)(arg0 + 0x1F1) = 1;
                *(int *)(arg0 + 0x208) = 0;
            } else if (*(float *)(arg0 + 0x38) > *(float *)(arg0 + 0x240)) {
                if (*(unsigned char *)(arg0 + 0x25C) > 1) {
                    *(unsigned char *)(arg0 + 0x1F1) = 2;
                    *(int *)(arg0 + 0x208) = 0;
                } else {
                    *(unsigned char *)(arg0 + 0x1F0) = 3;
                    *(int *)(arg0 + 0x208) = 0;
                }
            } else if (*(unsigned char *)(arg0 + 0x25C) == 2) {
                *(float *)(arg0 + 0x204) = 0.75f;
            }
        }
        break;
    case 2:
        step = 0.03125f;
        if (*(unsigned char *)(arg0 + 0x314) & 0x1F) {
            step *= 2.0f;
        }
        i = *(unsigned char *)(arg0 + 0x25C);
        lo = D_00248870[i];
        hi = D_0024886C[i];
        x = *(float *)(arg0 + 0x38) - step;
        *(float *)(arg0 + 0x38) = x;
        if (x <= hi) {
            *(float *)(arg0 + 0x38) = hi;
            if (*(unsigned char *)(arg0 + 0x23F) != 0) {
                *(unsigned char *)(arg0 + 0x1F0) = 1;
                *(unsigned char *)(arg0 + 0x25C) = *(unsigned char *)(arg0 + 0x25C) - 1;
                *(int *)(arg0 + 0x208) = 0;
                if (*(float *)(arg0 + 0x38) < *(float *)(arg0 + 0x240)) {
                    *(unsigned char *)(arg0 + 0x1F1) = 1;
                } else if (*(float *)(arg0 + 0x38) > *(float *)(arg0 + 0x240)) {
                    *(unsigned char *)(arg0 + 0x1F1) = 2;
                } else {
                    *(unsigned char *)(arg0 + 0x1F1) = 0;
                }
            } else {
                *(float *)(arg0 + 0x38) = 0.0f;
                *(unsigned char *)(arg0 + 0x1F0) = 3;
                *(unsigned char *)(arg0 + 0x1F1) = 0;
            }
        }
        *(float *)0x70003A20 = 1.0f - (hi - *(float *)(arg0 + 0x38)) / (hi - lo);
        *(float *)(arg0 + 0x204) = 1.0f + 0.5f * *(float *)0x70003A20;
        break;
    case 6:
        *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x38) - 0.05f;
        if (*(float *)(arg0 + 0x38) < 0.0f) {
            *(float *)(arg0 + 0x38) = 0.0f;
        }
        break;
    }
}
