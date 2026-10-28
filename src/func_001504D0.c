// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// State-1 handler (sibling of func_00150260 / func_00150600), stepped by the
// byte self+6. Step 0: advances the step, sets self+0xB4 = -90.0f, seeds the
// ctl physics block (byte +0x64 = 6, +0x38 = 3.5f, +0x3C = -0.045f as the
// bit pattern 0xBD3851EC, +0x40 = -1.0f, clears +0x48/+0x34/+0x2C), starts
// clip 5 via anim_clip_init(self, 5, 0, 0), marks the linked object at
// self+0x20 (+5 = 3) if present, and fires func_001FBD50 events 0x871 and
// 0x87D (1000.0f). Step 1: once flag 0x1000 of ctl+0 is set and self+0xB4 has
// fallen to <= -130.0f, sets self+4 = 1, clears the state/step bytes +5/+6
// and the short +0x36, and zeroes ctl+0x3C/+0x38/+0x48/+0x40.
extern void anim_clip_init(int self, int clip, float a, float b);
extern void func_001FBD50(char *p, int a, int b, float f);

void func_001504D0(char *arg0, char *arg1) {
    unsigned char st;
    char *p;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(float *)(arg0 + 0xB4) = -90.0f;
        *(unsigned char *)(arg1 + 0x64) = 6;
        *(float *)(arg1 + 0x38) = 3.5f;
        *(int *)(arg1 + 0x3C) = 0xBD3851EC;
        *(float *)(arg1 + 0x40) = -1.0f;
        *(int *)(arg1 + 0x48) = 0;
        *(int *)(arg1 + 0x34) = 0;
        *(int *)(arg1 + 0x2C) = 0;
        anim_clip_init((int)arg0, 5, 0.0f, 0.0f);
        p = *(char **)(arg0 + 0x20);
        if (p != 0) {
            *(char *)(p + 5) = 3;
        }
        func_001FBD50(arg0, 0x871, 0, 1000.0f);
        func_001FBD50(arg0, 0x87D, 0, 1000.0f);
        break;
    case 1:
        if (*(int *)(arg1 + 0) & 0x1000) {
            if (*(float *)(arg0 + 0xB4) <= -130.0f) {
                *(char *)(arg0 + 4) = 1;
                *(char *)(arg0 + 5) = 0;
                *(unsigned char *)(arg0 + 6) = 0;
                *(short *)(arg0 + 0x36) = 0;
                *(int *)(arg1 + 0x3C) = 0;
                *(int *)(arg1 + 0x38) = 0;
                *(int *)(arg1 + 0x48) = 0;
                *(int *)(arg1 + 0x40) = 0;
            }
        }
        break;
    }
}
