// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-state driver dispatched on the state byte at arg0+6 (switch 0/1).
// State 0: bumps the state byte, zeroes the long at arg0+0xC4 and several
// arg1 fields (0x04, 0x2C, 0x34), seeds the physics block (arg1+0x64 = 6,
// arg1+0x38 = 2.5f, arg1+0x3C = -0.04f bits 0xBD23D70A, arg1+0x48 = -0.025f
// bits 0xBCCCCCCD), clears arg1+0x67, clamps arg1+0x40 to >= 0, then kicks a
// clip via anim_clip_init(self, 6, 10.0f, 0.0f) and, if the heading short at
// arg0+0x34 is set and not flagged 0x8000, marks arg0+0 = 1 and zeroes the
// 0x36 short; finally fires func_001FBD50(self, 0x87D, 0, 1000.0f).
// State 1: clamps the velocity at arg1+0x40 to >= -4.0f (zeroing arg1+0x34);
// once decelerating (arg1+0x38 < 0) and not yet latched (arg1+0x67 == 0),
// latches it and propagates 2 into the linked object at arg0+0x20 (+5); if the
// one-shot at arg1+4 is clear, still decelerating, and the value at arg0+0xB4
// is <= 88.0f, fires func_001FBD50(self, 0x872, 0, 1000.0f); when flagged
// (arg1+0 & 0x1000) and arg0+0xB4 <= -130.0f, resets the state byte and physics
// fields and sets the finish codes at arg0+4/+5 from the heading short.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); pinned 991202 reaches 88.4%.
// Keys: (1) anim_clip_init's real signature is (int self, int clip, float a,
// float b) -- the trailing 0.0f emits the mtc1 zero,$f13 pair-clear; (2) the
// 0/1 dispatch is a switch (not if/else) to reproduce the case-0 branch-likely
// (beql), and the State-1 tail is written as if/else-if (h==0 first) so mwcc
// emits the target's `bnez h` with the v1=1 slot-filler. objdiff 100.0.
extern void anim_clip_init(int self, int clip, float a, float b);
extern void func_001FBD50(char *p, int a, int b, float f);

void func_00150260(char *arg0, char *arg1) {
    unsigned char st;
    short h;
    char *p;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(int *)(arg0 + 0xC4) = 0;
        *(int *)(arg1 + 4) = 0;
        *(char *)(arg1 + 0x64) = 6;
        *(float *)(arg1 + 0x38) = 2.5f;
        *(int *)(arg1 + 0x3C) = 0xBD23D70A;
        *(int *)(arg1 + 0x48) = 0xBCCCCCCD;
        *(char *)(arg1 + 0x67) = 0;
        if (!(*(float *)(arg1 + 0x40) <= 0.0f)) {
            *(int *)(arg1 + 0x40) = 0;
        }
        *(int *)(arg1 + 0x34) = 0;
        *(int *)(arg1 + 0x2C) = 0;
        anim_clip_init((int)arg0, 6, 10.0f, 0.0f);
        if (*(short *)(arg0 + 0x34) != 0) {
            if (!(*(short *)(arg0 + 0x36) & 0x8000)) {
                *(char *)(arg0 + 0) = 1;
                *(short *)(arg0 + 0x36) = 0;
            }
        }
        func_001FBD50(arg0, 0x87D, 0, 1000.0f);
        break;
    case 1:
        if (*(float *)(arg1 + 0x40) < -4.0f) {
            *(int *)(arg1 + 0x40) = 0xC0800000;
            *(int *)(arg1 + 0x34) = 0;
        }
        if (*(float *)(arg1 + 0x38) < 0.0f && *(char *)(arg1 + 0x67) == 0) {
            *(char *)(arg1 + 0x67) = 1;
            p = *(char **)(arg0 + 0x20);
            if (p != 0) {
                *(char *)(p + 5) = 2;
            }
        }
        if (*(int *)(arg1 + 4) == 0 && *(float *)(arg1 + 0x38) < 0.0f && *(float *)(arg0 + 0xB4) <= 88.0f) {
            *(int *)(arg1 + 4) = 1;
            func_001FBD50(arg0, 0x872, 0, 1000.0f);
        }
        if ((*(int *)(arg1 + 0) & 0x1000) && *(float *)(arg0 + 0xB4) <= -130.0f) {
            *(unsigned char *)(arg0 + 6) = 0;
            *(int *)(arg1 + 0x3C) = 0;
            *(int *)(arg1 + 0x38) = 0;
            *(int *)(arg1 + 0x48) = 0;
            *(int *)(arg1 + 0x40) = 0;
            h = *(short *)(arg0 + 0x34);
            if (h != 0 && !(*(short *)(arg0 + 0x36) & 0x8000)) {
                *(char *)(arg0 + 4) = 1;
                *(char *)(arg0 + 5) = 0;
                *(short *)(arg0 + 0x36) = 0;
            } else if (h == 0) {
                *(char *)(arg0 + 5) = 2;
            } else {
                *(char *)(arg0 + 5) = 1;
            }
        }
        break;
    }
}
