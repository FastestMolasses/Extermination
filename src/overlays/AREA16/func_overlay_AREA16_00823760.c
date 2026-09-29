// NEARMISS func_overlay_AREA16_00823760 (98.27%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x008237A0 (splat/link name 00823760; overlay code is
// linked 0x40 below where it runs), 0x208 bytes.
// Role (read from the instructions): state 0: +0x1F0 = 0, +0x1F4 / +0x1F8 =
//  rand fractions, state 1 (falls through). State 1: t = 1 - +0x1F0 / 1.1
//  (kept at 0x70003A20), colour from func_001281C0(80t, 80t, 160t) ->
//  func_001CD520(0, 2, self + 0x100, .., colour, 7, 7, 5), func_001D04B0(self
//  + 0xD0, 2, 0x828DD0, +0x1F0, +0x1F4) and (.., 1, 0x828E60, +0x1F0, +0x1F8);
//  +0x1F0 += 0.02, state 3 past 1.1. States 2/3 func_001AFC10.
// Divergence: the state byte is loaded into a2 before the self copy in the
//  original and into a0 after it in mwcc 2.3.3; extra parameters, a switch
//  local and declaration orders (perm search) do not move it. The float
//  arguments were aligned with an int-staged 7.0 (idiom-24 style).
extern float D_70003A20;
extern char D_overlay_AREA16_00828DD0[];
extern char D_overlay_AREA16_00828E60[];
extern int func_00122BB8(void);
extern int func_001281C0(float f);
extern int func_001CD520(int bucket, int mode, void *world, unsigned long long giftag,
                         unsigned int rgba, float w, float h, float zbias);
extern void func_001D04B0(void *m, int a1, void *tbl, float f12, float f13);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_00823760(unsigned char *self) {
    float *blk = (float *)(self + 0x1F0);
    float a;
    float t;
    unsigned int c;
    switch (self[4]) {
    case 0:
        blk[0] = 0.0f;
        blk[1] = (float)func_00122BB8() / 2147483648.0f;
        blk[2] = (float)func_00122BB8() / 2147483648.0f;
        self[4] = 1;
    case 1:
        D_70003A20 = 1.0f - blk[0] / 1.1f;
        a = 80.0f * (t = D_70003A20);
        c = func_001281C0(a);
        c |= func_001281C0(a) << 8;
        c |= func_001281C0(160.0f * t) << 16;
        /* matching device: the second 7.0 staged through an int */
        { int zi = 7; float s = (float)zi; func_001CD520(0, 2, self + 0x100, 0x20045BA5154222DCULL, c, 7.0f, s, 5.0f); }
        func_001D04B0(self + 0xD0, 2, D_overlay_AREA16_00828DD0, blk[0], blk[1]);
        func_001D04B0(self + 0xD0, 1, D_overlay_AREA16_00828E60, blk[0], blk[2]);
        blk[0] += 0.02f;
        if (blk[0] > 1.1f) {
            self[4] = 3;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
