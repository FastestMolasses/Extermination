// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Four-step actor sub-state handler on the step byte self+6.
// Step 0: advances, clears the anim flags ctl+0x58 and ctl+0x3C, speed
// ctl+0x34 = 1.0, starts clip 0x30; falls through to step 1.
// Step 1: when the clip ends (ctl+0x58 bit 0x1000) advances and starts clip 0x31.
// Step 2: sets ctl+0x6A = 100; when func_001333F0(self, ctl) returns nonzero
// advances, clears ctl+0x52, sets the timer ctl+0x56 = 120, starts clip 0x32.
// Step 3: when the clip ends: state self+5 = 1, step 0, ctl+0x6A = 90.
// Every clip starts with blend 1.0 at frame 0.

extern void anim_clip_init(char *self, int clip, float a, float b);
extern int func_001333F0(char *arg0, char *arg1);

void func_00131510(char *self, char *ctl) {
    switch (*(unsigned char *)(self + 6)) {
    case 0:
        *(unsigned char *)(self + 6) += 1;
        *(short *)(ctl + 0x58) = 0;
        *(float *)(ctl + 0x3C) = 0.0f;
        *(float *)(ctl + 0x34) = 1.0f;
        anim_clip_init(self, 0x30, 1.0f, 0.0f);
    case 1:
        if (*(unsigned short *)(ctl + 0x58) & 0x1000) {
            *(unsigned char *)(self + 6) += 1;
            anim_clip_init(self, 0x31, 1.0f, 0.0f);
        }
        break;
    case 2:
        *(unsigned char *)(ctl + 0x6A) = 100;
        if (func_001333F0(self, ctl) != 0) {
            *(unsigned char *)(self + 6) += 1;
            *(short *)(ctl + 0x52) = 0;
            *(short *)(ctl + 0x56) = 120;
            anim_clip_init(self, 0x32, 1.0f, 0.0f);
        }
        break;
    case 3:
        if (*(unsigned short *)(ctl + 0x58) & 0x1000) {
            *(unsigned char *)(self + 5) = 1;
            *(unsigned char *)(self + 6) = 0;
            *(unsigned char *)(ctl + 0x6A) = 90;
        }
        break;
    }
}
