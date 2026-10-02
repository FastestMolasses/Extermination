// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x00823860 (splat/link name 00823820; overlay code
//  is linked 0x40 below where it runs), 0x168 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: effect object (no static reference). State 0 as 0x8235E0. State 1: a
//  func_001CFB50 packet (+0xD0, +0x1F4, (float)seed, 1.0, 1e-6, 100) with its
//  +0x54 word negated, drawn with tables 0x826770 and 0x826800 (bucket 5);
//  +0x1F4 += 0.01, state 3 past 1.5.
extern char D_overlay_AREA17_00826770[];
extern char D_overlay_AREA17_00826800[];
extern void func_001029C0(void *m);
extern void func_00102918(void *a, void *b, void *c);
extern int func_00122BB8(void);
extern int func_001CCF70(void *p);
extern void func_001CFB50(void *pkt, int a1, void *m, float f12, float f13, float f14, float f15, float f16);
extern void func_001CFBE0(int h, int a1, void *a2, void *a3, int t0);

void func_overlay_AREA17_00823820(unsigned char *self) {
    float pkt[24];
    int h;
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_001029C0(self + 0xD0);
        func_00102918(self + 0xD0, self + 0xD0, self + 0xB0);
        *(int *)blk = func_00122BB8();
        *(float *)(blk + 4) = 0.0f;
        self[4] = 1;
        break;
    case 1:
        h = func_001CCF70(self + 0x100);
        func_001CFB50(pkt, 0, self + 0xD0, *(float *)(blk + 4), (float)*(int *)blk, 1.0f, 1e-6f, 100.0f);
        pkt[0x15] = -pkt[0x15];
        func_001CFBE0(h, 5, D_overlay_AREA17_00826770, pkt, 0);
        func_001CFBE0(h, 5, D_overlay_AREA17_00826800, pkt, 0);
        *(float *)(blk + 4) += 0.01f;
        if (!(*(float *)(blk + 4) <= 1.5f)) {
            self[4] = 3;
        }
        break;
    case 2:
    case 3:
        break;
    }
}
