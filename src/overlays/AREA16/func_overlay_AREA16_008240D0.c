// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00824110 (splat/link name 008240D0; overlay code is
// linked 0x40 below where it runs), 0x308 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: state 0: +0x1F0 = +0x1F4 = 0, +0x1F8 = rand fraction, state 1, +5 = 0,
//  func_001029C0(self + 0xD0) (falls through). State 1 by +5: 1 fades +0x1F4
//  in by 0.2 (starting +0x1F0 at 1) at (314.8, 176.8, 428.1); 2 fades in by
//  1/30 at (309.1, 184.6, 428.6); 3 fades out by 1/30 and resets +5 at 0; 0x63
//  fades out by 0.1 then state 3. While +0x1F4 != 0 draws a packet
//  (func_001CCF70(self + 0x100), func_001CFAE0 .., 0.05, func_001CFBE0 table
//  0x8292B0) and +0x1F0 += 0.015, wrapping past 2. States 2/3 func_001AFC10.
extern char D_overlay_AREA16_008292B0[];
extern int func_00122BB8(void);
extern void func_001029C0(void *m);
extern int func_001CCF70(void *p);
extern void func_001CFAE0(void *dst, int a1, void *src, float f12, float f13, float f14, float f15);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int t0);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_008240D0(unsigned char *self) {
    float *blk = (float *)(self + 0x1F0);
    int pkt[24];
    int h;
    switch (self[4]) {
    case 0:
        blk[0] = 0.0f;
        blk[1] = 0.0f;
        blk[2] = (float)func_00122BB8() / 2147483648.0f;
        self[4] = 1;
        self[5] = 0;
        func_001029C0(self + 0xD0);
    case 1:
        switch (self[5]) {
        case 0:
            break;
        case 1:
            blk[1] += 0.2f;
            if (blk[1] > 1.0f) {
                blk[1] = 1.0f;
            }
            if (blk[0] == 0.0f) {
                blk[0] = 1.0f;
            }
            *(float *)(self + 0x100) = 314.8f;
            *(float *)(self + 0x104) = 176.8f;
            *(float *)(self + 0x108) = 428.1f;
            *(float *)(self + 0x10C) = 1.0f;
            break;
        case 2:
            blk[1] += 0.033333335f;
            if (blk[1] > 1.0f) {
                blk[1] = 1.0f;
            }
            *(float *)(self + 0x100) = 309.1f;
            *(float *)(self + 0x104) = 184.6f;
            *(float *)(self + 0x108) = 428.6f;
            *(float *)(self + 0x10C) = 1.0f;
            break;
        case 3:
            blk[1] -= 0.033333335f;
            if (blk[1] < 0.0f) {
                blk[1] = 0.0f;
                blk[0] = 0.0f;
                self[5] = 0;
            }
            break;
        case 0x63:
            blk[1] -= 0.1f;
            if (blk[1] < 0.0f) {
                self[4] = 3;
            }
            break;
        }
        if (blk[1] != 0.0f) {
            h = func_001CCF70(self + 0x100);
            func_001CFAE0(pkt, 0, self + 0xD0, blk[0], blk[2], blk[1], 0.05f);
            func_001CFBE0(h, 1, D_overlay_AREA16_008292B0, pkt, 0);
            blk[0] += 0.015f;
            if (blk[0] > 2.0f) {
                blk[0] -= 1.0f;
            }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
