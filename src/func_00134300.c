// NEARMISS func_00134300  (vram 0x00134300, 0x1F8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.97% via mwcc 2.4 (mwcc24) (-O4,p -sdatathreshold 0) (was an asm-word body, matching
// wave 2026-10-28; mwcc 2.3.3 gives 92.17%: it hoists the float constants, the original loads each
// right before its use as 2.4 does, and the neighbour func_00134090 is also a 2.4 file).
// Remaining differences: register colouring only. The original keeps the shared constant 1 in a3,
// the flags halfword in a2 and the phase word in t0 (ours a2/a1/a3), and holds the fade value in f3
// with 90.0 in f2 (ours swapped). Tried: int vs short flags, phase as a local or field reads,
// locals at block or function top in two orders, parameter copies (self/ctl, both orders), a
// state local, case order, if/else form, phase switch, compound field update, a 90.0 local,
// sdatathreshold 4. A missing 2.4.x/3.0 point release may be the real cause.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc24
// CFLAGS: -O4,p -sdatathreshold 0

// Two-step actor sub-state handler on the step byte self+6 (a fade-out / death sequence).
// Step 0: advances, clears ctl+4 (phase), ctl+0x30 and ctl+0x2C, and starts clip 6
// (blend 10.0) when bits 12..15 of rand() are nonzero, else clip 0 (blend 5.0).
// Step 1: only while the halfword self+0x2C has no bits other than 0x8000:
//   phase 1: ctl+0x3C moves 3% of the way toward 90.0 each tick; when ctl+0x5C
//     is set, func_001E04E0(self+0xB0, alpha, value) with alpha =
//     float_to_int(255 * (90 - value) / 90); once the value reaches 80.0 the
//     phase advances.
//   phase 0: when bit 0x8000 is clear and self+0x3C <= 130.0, the phase
//     advances, ctl+0x3C = 10.0 and event 0x84B fires via
//     func_001FBD50(self, 0x84B, 0, 450.0).
// Then, when ctl+0 has bit 0x1000 or the halfword ctl+0x56 is nonzero:
// state self+5 = 1 and step 0.
extern int func_00122BB8(void);
extern void anim_clip_init(char *self, int clip, float a, float b);
extern int float_to_int(float f);
extern void func_001E04E0(void *pos, int alpha, float t);
extern void func_001FBD50(char *p, int a, int b, float f);

void func_00134300(char *self, char *ctl) {
    switch (*(unsigned char *)(self + 6)) {
    case 0:
        *(unsigned char *)(self + 6) += 1;
        *(int *)(ctl + 4) = 0;
        *(int *)(ctl + 0x30) = 0;
        *(int *)(ctl + 0x2C) = 0;
        if ((func_00122BB8() >> 12) & 0xF) {
            anim_clip_init(self, 6, 10.0f, 0.0f);
        } else {
            anim_clip_init(self, 0, 5.0f, 0.0f);
        }
        break;
    case 1: {
        short flags = *(short *)(self + 0x2C);
        if ((flags & ~0x8000) == 0) {
            int phase = *(int *)(ctl + 4);
            if (phase == 1) {
                float cur = *(float *)(ctl + 0x3C);
                float next = cur + 0.03f * (90.0f - cur);
                int alpha;
                *(float *)(ctl + 0x3C) = next;
                alpha = float_to_int((255.0f * (90.0f - next)) / 90.0f);
                if (*(signed char *)(ctl + 0x5C) != 0) {
                    func_001E04E0(self + 0xB0, alpha, next);
                }
                if (!(*(float *)(ctl + 0x3C) < 80.0f)) {
                    *(int *)(ctl + 4) += 1;
                }
            } else if (phase == 0 && !(flags & 0x8000) && *(float *)(self + 0x3C) <= 130.0f) {
                *(int *)(ctl + 4) = phase + 1;
                *(float *)(ctl + 0x3C) = 10.0f;
                func_001FBD50(self, 0x84B, 0, 450.0f);
            }
        }
        if ((*(int *)ctl & 0x1000) || *(short *)(ctl + 0x56) != 0) {
            *(unsigned char *)(self + 5) = 1;
            *(unsigned char *)(self + 6) = 0;
        }
        break;
    }
    }
}
