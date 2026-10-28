// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Actor sub-state handler on the step byte self+6, always followed by
// func_00132490(self, ctl).
// Step 0: advances the step, sets the clip speed ctl+0x34 = 1.0, clears
// ctl+0x3C and starts clip 0xB (blend 5.0); falls through to step 1.
// Step 1: when bit 0x1000 of the halfword ctl+0x58 is set (clip ended): state
// self+5 = 4, step 0, ctl+0x3C = 0.4, ctl+0x38 = 2.1, and fires event 0x7D3
// via func_001FBD50(self, 0x7D3, 0, 300.0).

extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001FBD50(char *p, int a, int b, float f);
extern void func_00132490(char *p, char *e);

void func_0012F980(char *self, char *ctl) {
    switch (*(unsigned char *)(self + 6)) {
    case 0:
        *(unsigned char *)(self + 6) += 1;
        *(float *)(ctl + 0x34) = 1.0f;
        *(float *)(ctl + 0x3C) = 0.0f;
        anim_clip_init(self, 0xB, 5.0f, 0.0f);
    case 1:
        if (*(unsigned short *)(ctl + 0x58) & 0x1000) {
            *(unsigned char *)(self + 5) = 4;
            *(unsigned char *)(self + 6) = 0;
            *(float *)(ctl + 0x3C) = 0.4f;
            *(float *)(ctl + 0x38) = 2.1f;
            func_001FBD50(self, 0x7D3, 0, 300.0f);
        }
        break;
    }
    func_00132490(self, ctl);
}
