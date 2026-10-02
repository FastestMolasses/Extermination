// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Actor sub-state handler (sub-step byte self+6, control block ctl).
// Step 0 advances the step, clears ctl+0x50 / +0x4C and starts clip 2 (or clip 1
// when bits 19..21 of rand() are all 0) through anim_clip_init(self, clip, 5.0,
// 0.0). Step 1 plays sound 0x833 (func_001FBD50(self, 0x833, 0, 300.0)) when
// the clip id at self+0x2C (bit 15 ignored) is 1 at frame 80.0 (self+0x3C), runs
// func_001469B0(self, ctl), then reads ctl's flags: with bit 7 of ctl+0x7F set
// the step returns to 0 when bit 12 of ctl+0x30 is set; otherwise bit 12 of
// ctl+0x30, a nonzero ctl+0x64 or a nonzero ctl+0x7F gives self+5 = 1 and step
// 0. Failing those, ctl+0x78 nonzero with ctl+0x60 zero gives self+5 = 5 and
// step 0.
extern int func_00122BB8(void);
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001FBD50(char *p, int a, int b, float f);
extern void func_001469B0(void *arg0, void *arg1);

void func_00142330(char *self, char *ctl) {
    unsigned char st = *(unsigned char *)(self + 6);
    signed char flags;
    switch (st) {
    case 0:
        *(unsigned char *)(self + 6) = st + 1;
        *(int *)(ctl + 0x50) = 0;
        *(int *)(ctl + 0x4C) = 0;
        if ((func_00122BB8() >> 19) & 7) {
            anim_clip_init(self, 2, 5.0f, 0.0f);
        } else {
            anim_clip_init(self, 1, 5.0f, 0.0f);
        }
        break;
    case 1:
        if ((*(short *)(self + 0x2C) & 0xFFFF7FFF) == 1 && *(float *)(self + 0x3C) == 80.0f) {
            func_001FBD50(self, 0x833, 0, 300.0f);
        }
        func_001469B0(self, ctl);
        flags = *(signed char *)(ctl + 0x7F);
        if (flags & 0x80) {
            if (*(int *)(ctl + 0x30) & 0x1000) {
                *(unsigned char *)(self + 6) = 0;
                break;
            }
        } else if ((*(int *)(ctl + 0x30) & 0x1000) || *(short *)(ctl + 0x64) != 0 || flags != 0) {
            *(unsigned char *)(self + 5) = 1;
            *(unsigned char *)(self + 6) = 0;
            break;
        }
        if (*(signed char *)(ctl + 0x78) != 0 && *(short *)(ctl + 0x60) == 0) {
            *(unsigned char *)(self + 5) = 5;
            *(unsigned char *)(self + 6) = 0;
        }
        break;
    }
}
