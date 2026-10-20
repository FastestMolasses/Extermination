// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Three-state actor tick keyed on the sub-state byte self[6]; arg1 is the
// actor's state block.
// State 0: advance to 1, set arg1+0x40 = 1.0f, zero arg1+0x48/+0x50/+0x4C, and
// start clip 0 with anim_clip_init(self, 0, 5.0f, 0.0f).
// State 1: while the self+0x56 short is nonzero, it is cleared once the
// float at arg1+0x5C is <= its value, or when self[0xA] has bit 1.
// When it is zero, run func_001418F0(self, arg1) and, if the arg1+0x70 short
// is set, advance to state 2 and start clip 1 (anim_clip_init(self, 1, 0, 0)).
// State 2: once arg1+0x30 has bit 0x1000, switch the actor to
// state 4 (self[5] = 4, self[6] = 0). Other sub-states do nothing.
//
// Byte-matched (objdiff 100.0 with mwcc 2.3.3 and 2.4; 991202 reaches 91.7).
// Matching keys: func_001418F0 takes (self, arg1), so both parameters stay
// live and the state byte lands in a2 (the sibling func_0013D980 had the same
// prologue residual from a missing argument); and the `z, (z = 0.0f)` pair
// gives the target's float argument order for the second anim_clip_init.
extern void anim_clip_init(int self, int clip, float a, float b);
extern void func_001418F0(unsigned char *self, unsigned char *arg1);

void func_0013D850(unsigned char *self, unsigned char *arg1) {
    float z;
    unsigned char st = self[6];

    switch (st) {
    case 0:
        self[6] = st + 1;
        *(int *)(arg1 + 0x40) = 0x3F800000;
        *(int *)(arg1 + 0x48) = 0;
        *(int *)(arg1 + 0x50) = 0;
        *(int *)(arg1 + 0x4C) = 0;
        anim_clip_init((int)self, 0, 5.0f, 0.0f);
        break;
    case 1:
        if (*(short *)(self + 0x56) != 0) {
            if (*(float *)(arg1 + 0x5C) <= (float)*(short *)(self + 0x56) || (self[0xA] & 1)) {
                *(short *)(self + 0x56) = 0;
            }
        } else {
            func_001418F0(self, arg1);
            if (*(short *)(arg1 + 0x70) != 0) {
                self[6]++;
                /* Both float arguments are 0.0f in the original: the zero is
                   materialized once and copied; the pair only fixes the order. */
                anim_clip_init((int)self, 1, z, (z = 0.0f));
            }
        }
        break;
    case 2:
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            self[5] = 4;
            self[6] = 0;
        }
        break;
    }
}
