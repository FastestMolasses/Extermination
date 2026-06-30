// NEARMISS func_0016BE40  (vram 0x0016BE40, 0x134 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.60% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body 100% correct on both builds (96.6% on 2.3.3, only 5 prologue instructions differ). Residual is a param-load register-allocation / evaluation-order permutation: target loads D_00248650 value into a1 and computes &sp20 into a2 (value-arg evaluated first, register kept alive across the sp30 sto...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Per-frame anim-blend clamp. func_001751A0 seeds two float[4] limit
// vectors (lo=D_00248650 -> sp20, hi=D_00248660 -> sp30) keyed off the
// actor at arg0; index byte at arg0+0x23F selects the active channel.
// mode (arg0+0x24C): 0 = ease the value at arg0+0x38 UP toward sp20[idx]
// by 1/16 of the target (clamped not to overshoot); 1 = ease DOWN toward
// sp30[idx] by 1/8 of the gap (clamped not to undershoot).
typedef struct { float v[4]; } Q;
extern Q D_00248650;
extern Q D_00248660;
extern void func_001751A0(unsigned char *a0, Q a1, Q *a2);

void func_0016BE40(unsigned char *arg0) {
    Q sp20;
    Q sp30;
    int mode;
    float cur;
    float lo;
    float hi;

    sp30 = D_00248660;
    sp20 = D_00248650;
    func_001751A0(arg0, D_00248650, &sp20);
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
