// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Arc-motion sub-state (label inferred) (sub-state 0 of func_0014EDD0), stepped by self+6; the
// frame counter ctx+4 is bumped on every call. State 0: reset (counter ctx+4,
// self+0xC4, ctx+0x52 = 0, ctx+0x20 = 1.0f) and fall into state 1. State 1:
// launch: clear the sound flags ctx+0xC, vertical speed ctx+0x38 = 3.0,
// ctx+0x40 = 1.0, random drift ctx+0x2C in [-0.5, 0.5], random gravities
// ctx+0x3C in [-0.020, -0.018] and ctx+0x48 in [-0.0105, -0.0065], clear
// ctx+0x64/ctx+0x67, func_00150DA0(self, ctx), height self+0xB4 = -120,
// anim_clip_init(self, 0, 0, 0), and mark the linked object (self+0x20)+5 = 1.
// State 2: drift self+0xB0 by ctx+0x2C; clamp ctx+0x38 and ctx+0x40 at -4
// (zeroing their gravities); ctx+0x64 = (ctx+0x40 < 0). At the apex (ctx+0x38
// first goes negative, latched by bit 0x80 of ctx+0x67) a 50% chance calls
// func_00150F40 and the linked object gets +5 = 2. Sounds: falling and at or
// below height 90 -> 0x872 once (bit 2); rising and not below -95 -> 0x871
// once (bit 1) plus a 1-in-4 chance of 0x873. When ctx+0 bit 0x1000 is set and
// height is at or below -160: next state with a random wait ctx+8 = 30..93.
// State 3: count ctx+8 down; at 0 go back to state 1, and when ctx+0x65 == 2
// (limit 2400 frames) with ctx+0x52 < 200 a 1-in-4 chance switches to sub-state
// 2; otherwise (limit 1200) sub-state 1 is entered once ctx+0x52 >= 200 or the
// frame counter reaches the limit.
extern int func_00122BB8(void);
extern void func_00150DA0(char *self, char *ctx);
extern void func_00150F40(unsigned char *arg0, unsigned char *arg1);
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001FBD50(char *p, int a, int b, float f);

void func_0014EEF0(char *self, char *ctx) {
    char *link;
    int limit;

    *(int *)(ctx + 4) = *(int *)(ctx + 4) + 1;
    switch (*(unsigned char *)(self + 6)) {
    case 0:
        *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
        *(int *)(self + 0xC4) = 0;
        *(int *)(ctx + 4) = 0;
        *(float *)(ctx + 0x20) = 1.0f;
        *(short *)(ctx + 0x52) = 0;
        /* fallthrough */
    case 1:
        *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
        *(int *)(ctx + 0xC) = 0;
        *(float *)(ctx + 0x38) = 3.0f;
        *(float *)(ctx + 0x40) = 1.0f;
        *(float *)(ctx + 0x2C) = (float)((func_00122BB8() >> 5) & 0xFF) / 255.0f - 0.5f;
        *(float *)(ctx + 0x3C) = -0.018f + -0.002f * ((float)((func_00122BB8() >> 18) & 0xFF) / 255.0f);
        *(float *)(ctx + 0x48) = -0.0065f + -0.004f * ((float)((func_00122BB8() >> 12) & 0xFF) / 255.0f);
        *(char *)(ctx + 0x64) = 0;
        *(char *)(ctx + 0x67) = 0;
        func_00150DA0(self, ctx);
        *(float *)(self + 0xB4) = -120.0f;
        anim_clip_init(self, 0, 0.0f, 0.0f);
        link = *(char **)(self + 0x20);
        if (link != 0) {
            *(char *)(link + 5) = 1;
        }
        break;
    case 2:
        *(float *)(self + 0xB0) += *(float *)(ctx + 0x2C);
        if (*(float *)(ctx + 0x38) < -4.0f) {
            *(float *)(ctx + 0x38) = -4.0f;
            *(float *)(ctx + 0x3C) = 0.0f;
        }
        if (*(float *)(ctx + 0x40) < -4.0f) {
            *(float *)(ctx + 0x40) = -4.0f;
            *(float *)(ctx + 0x48) = 0.0f;
        }
        if (!(*(float *)(ctx + 0x40) < 0.0f)) {
            *(char *)(ctx + 0x64) = 0;
        } else {
            *(char *)(ctx + 0x64) = 1;
        }
        if (*(float *)(ctx + 0x38) < 0.0f && !(*(char *)(ctx + 0x67) & 0x80)) {
            *(char *)(ctx + 0x67) = 0x80;
            if ((func_00122BB8() >> 15) & 1) {
                func_00150F40((unsigned char *)self, (unsigned char *)ctx);
            }
            link = *(char **)(self + 0x20);
            if (link != 0) {
                *(char *)(link + 5) = 2;
            }
        }
        if (*(float *)(ctx + 0x38) < 0.0f) {
            if (!(*(int *)(ctx + 0xC) & 2) && *(float *)(self + 0xB4) <= 90.0f) {
                *(int *)(ctx + 0xC) = *(int *)(ctx + 0xC) | 2;
                func_001FBD50(self, 0x872, 0, 1000.0f);
            }
        } else {
            if (!(*(int *)(ctx + 0xC) & 1) && !(*(float *)(self + 0xB4) < -95.0f)) {
                *(int *)(ctx + 0xC) = *(int *)(ctx + 0xC) | 1;
                func_001FBD50(self, 0x871, 0, 1000.0f);
                if (((func_00122BB8() >> 14) & 3) == 0) {
                    func_001FBD50(self, 0x873, 0, 800.0f);
                }
            }
        }
        if ((*(int *)ctx & 0x1000) && *(float *)(self + 0xB4) <= -160.0f) {
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
            *(int *)(ctx + 8) = ((func_00122BB8() >> 14) & 0x3F) + 30;
        }
        break;
    case 3:
        if (*(int *)(ctx + 8) != 0) {
            *(int *)(ctx + 8) = *(int *)(ctx + 8) - 1;
            break;
        }
        *(unsigned char *)(self + 6) = 1;
        if (*(char *)(ctx + 0x65) != 2) {
            limit = 0x4B0;
        } else {
            limit = 0x960;
            if (*(short *)(ctx + 0x52) < 200 && ((func_00122BB8() >> 7) & 3) == 0) {
                *(unsigned char *)(self + 5) = 2;
                *(unsigned char *)(self + 6) = 0;
                break;
            }
        }
        if (*(short *)(ctx + 0x52) >= 200 || *(int *)(ctx + 4) >= limit) {
            *(unsigned char *)(self + 5) = 1;
            *(unsigned char *)(self + 6) = 0;
        }
        break;
    }
}
