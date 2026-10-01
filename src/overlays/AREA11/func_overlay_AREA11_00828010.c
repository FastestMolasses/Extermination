// NEARMISS func_overlay_AREA11_00828010 (94.17%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x00828050 (splat/link name 00828010; overlay code is
// linked 0x40 below where it runs), 0x120 bytes.
// Role: script callback (self, st) that moves +0xB4 and the player together.
// st +4 0: +0x2EC = 0,
// sound 0x452 and +0x2E8 = 0.26666668 when D_0081083A is set, else sound
// 0x453 and +0x2E8 = -0.26666668; st +4 += 1, returns 0. st +4 1: adds
// +0x2E8 to +0xB4, the player's y (D_00810354) and D_008105E4, calls
// func_001C6380 and counts +0x2EC; returns 1 at 150 frames, else 0. Other
// values return 1.
// Divergence: (1) the default case: the original loads 1 into v0 again in
//  the branch delay slot; mwcc 2.3.3 reuses the 1 that the state compare left
//  in v0 and leaves a nop. (2) FPR assignment of the two global adds (the
//  original holds D_008105E4 in f1 and the sum in f0). (3) The frame test:
//  the original branches on a set-less-than into $at with a 0 / 1 return
//  pair; mwcc 2.3.3 selects with movn (the < 150 form) or keeps the compare
//  in v0 (the >= 150 form used here). Tried: every return shape with and
//  without a result variable, all 27 spellings of the three float adds,
//  arrays for the globals, mwcc 2.4 and 991202. Equivalent to the original:
//  the same stores, calls, and return values on every path.
extern unsigned char D_0081083A;
extern float D_00810354;
extern float D_008105E4;
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern void func_001C6380(unsigned char *self);

int func_overlay_AREA11_00828010(unsigned char *self, unsigned char *st) {
    switch (st[4]) {
    case 0:
        *(int *)(self + 0x2EC) = 0;
        if (D_0081083A != 0) {
            func_001FBD50(self, 0x452, 0, 300.0f);
            *(float *)(self + 0x2E8) = 0.266666681f;
        } else {
            func_001FBD50(self, 0x453, 0, 300.0f);
            *(float *)(self + 0x2E8) = -0.266666681f;
        }
        st[4]++;
        return 0;
    case 1:
        *(float *)(self + 0xB4) += *(float *)(self + 0x2E8);
        D_00810354 = *(float *)(self + 0x2E8) + D_00810354;
        D_008105E4 = *(float *)(self + 0x2E8) + D_008105E4;
        func_001C6380(self);
        (*(int *)(self + 0x2EC))++;
        if (*(int *)(self + 0x2EC) >= 150) {
            return 1;
        }
        return 0;
    }
    return 1;
}
