// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.60% NEARMISS).
// Fixes read from the original instructions:
// - func_001751A0 takes one argument (the actor), as its byte-matched
//   definition shows; the extra "arguments" were the struct-copy temporaries.
// - the two table copies run lo (sp20) first, then hi (sp30).

// Per-frame anim-blend clamp. Copies two float[4] limit tables
// (lo=D_00248650 -> sp20, hi=D_00248660 -> sp30) to the stack, calls
// func_001751A0(actor), then the index byte at arg0+0x23F selects the
// active channel.
// mode (arg0+0x24C): 0 = ease the value at arg0+0x38 UP toward sp20[idx]
// by 1/16 of the target (clamped not to overshoot); 1 = ease DOWN toward
// sp30[idx] by 1/8 of the gap (clamped not to undershoot).
typedef struct { float v[4]; } Q;
extern Q D_00248650;
extern Q D_00248660;
extern void func_001751A0(unsigned char *a0);

void func_0016BE40(unsigned char *arg0) {
    Q sp20;
    Q sp30;
    int mode;
    float cur;
    float lo;
    float hi;

    sp20 = D_00248650;
    sp30 = D_00248660;
    func_001751A0(arg0);
    mode = *(int *)(arg0 + 0x24C);
    if (mode == 0) {
        cur = *(float *)(arg0 + 0x38);
        lo = sp20.v[*(unsigned char *)(arg0 + 0x23F)];
        if (cur < lo) {
            *(float *)(arg0 + 0x38) = cur + (lo / 16.0f);
            lo = sp20.v[*(unsigned char *)(arg0 + 0x23F)];
            if (*(float *)(arg0 + 0x38) <= lo) {
                return;
            }
            *(float *)(arg0 + 0x38) = lo;
        }
    } else if (mode == 1) {
        cur = *(float *)(arg0 + 0x38);
        hi = sp30.v[*(unsigned char *)(arg0 + 0x23F)];
        if (cur > hi) {
            *(float *)(arg0 + 0x38) = cur - ((cur - hi) / 8.0f);
            hi = sp30.v[*(unsigned char *)(arg0 + 0x23F)];
            if (*(float *)(arg0 + 0x38) < hi) {
                *(float *)(arg0 + 0x38) = hi;
            }
        }
    }
}
