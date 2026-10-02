// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA08 overlay, runtime 0x00824BC0 (splat/link name 00824B80; overlay code
//  is linked 0x40 below where it runs), 0x4FC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA08; lane OVLC).
// Role: sub 3 placement [4]: five flame columns. State 0: after
//  func_001B0FD0, func_001C6380, five rand fractions in +0x1F4.., a seed in
//  +0x1F0, +0x28 = 0, +0x2EC = -1. State 1: D_00275B40 record +0x74 goes
//  through func_001B1470(x - 0.02) (via 0x70003A20); func_001C6380,
//  func_001B1B70 and the +0x4C method; the looping sound 0x910 is kept in the
//  +0x2EC voice (the func_001FC3C0 pattern with func_001FBE80(self, voice,
//  100, 4096) / func_001FBD50(self, 0x910, 1, 100), checked every 10th
//  frame); each column draws a func_001CFAE0 packet from table 0x827DD0
//  (+0x90 for the fifth) at its point, the phase +0x1F4.. growing by 0.008
//  and wrapping above 2. State 3 and others func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
#define REC_B (*(unsigned char **)(D_00275B40 + 0xC))
extern char *D_00275B40;
extern int D_00281C30[];
extern int D_70003B68[];
extern short D_70003B8A[];
extern int D_00281B70[];
extern float D_700036A0[];
extern float D_700036D0[];
extern char D_overlay_AREA08_00827DD0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_00122BB8(void);
extern float func_001B1470(float a);
extern void func_001B1B70(unsigned char *self);
extern void func_0011A070(int voice);
extern int func_001FBE80(unsigned char *self, int voice, float f12, float f13);
extern int func_001FBD50(unsigned char *self, int id, int flags, float f12);
extern void func_001029C0(void *m);
extern int func_001CCF70(void *pos);
extern void func_001CFAE0(void *pkt, int a1, void *m, float f12, float f13, float f14, float f15);
extern void func_001CFBE0(int h, int a1, void *a2, void *a3, int t0);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA08_00824B80(unsigned char *self) {
    int pkt[24];
    int i;
    int k;
    int *hp;
    int h;
    int seed;
    unsigned char *blk = self + 0x1F0;
    unsigned char *p;
    int idx;
    int r;
    float f;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        for (i = 0, p = blk; i < 5; i++) {
            *(float *)(p + 4) = (float)func_00122BB8() / 2147483648.0f;
            p += 4;
        }
        *(int *)blk = func_00122BB8();
        *(short *)(self + 0x28) = 0;
        *(int *)(self + 0x2EC) = -1;
        break;
    case 1:
        f = *(float *)(REC_B + 0x74) - 0.02f;
        *(float *)0x70003A20 = f;
        *(float *)(REC_B + 0x74) = func_001B1470(f);
        func_001C6380(self);
        func_001B1B70(self);
        (*(ActorFn *)(self + 0x4C))(self);
        idx = *(int *)(self + 0x2EC);
        hp = (int *)(self + 0x1F0) + 0x3F;
        if (idx != -1) {
            if (D_00281C30[idx] != 0x910) {
                func_0011A070(idx);
                D_00281B70[*hp] = -1;
                *hp = -1;
            } else if ((D_70003B68[0] + D_70003B8A[0]) % 10 == 0) {
                r = func_001FBE80(self, idx, 100.0f, 4096.0f);
                *hp = r;
                if (r == -1) {
                    D_00281B70[*hp] = -1;
                }
            }
        } else if ((D_70003B68[0] + D_70003B8A[0]) % 10 == 0) {
            r = func_001FBD50(self, 0x910, 1, 100.0f);
            *hp = r;
            if (r != -1) {
                D_00281B70[*hp] = 0x910;
            }
        }
        seed = *(int *)blk;
        for (i = 0; i < 5; i++) {
            func_001029C0(D_700036A0);
            switch (i) {
            case 0:
                D_700036D0[0] = 137.0f;
                D_700036D0[1] = 225.0f;
                D_700036D0[2] = 156.0f;
                D_700036D0[3] = 1.0f;
                k = 0;
                break;
            case 1:
                D_700036D0[0] = 142.0f;
                D_700036D0[1] = 225.0f;
                D_700036D0[2] = 169.0f;
                D_700036D0[3] = 1.0f;
                k = 0;
                break;
            case 2:
                D_700036D0[0] = 154.0f;
                D_700036D0[1] = 225.0f;
                D_700036D0[2] = 164.0f;
                D_700036D0[3] = 1.0f;
                k = 0;
                break;
            case 3:
                D_700036D0[0] = 150.0f;
                D_700036D0[1] = 225.0f;
                D_700036D0[2] = 151.0f;
                D_700036D0[3] = 1.0f;
                k = 0;
                break;
            case 4:
                D_700036D0[0] = 146.0f;
                D_700036D0[1] = 135.0f;
                D_700036D0[2] = 160.0f;
                D_700036D0[3] = 1.0f;
                k = 1;
                break;
            }
            h = func_001CCF70(D_700036D0);
            f = (float)((seed >> 16) & 0xFFFF);
            f = f / 65535.0f;
            f += 0.0001f;
            seed = seed * 37 + 11;
            func_001CFAE0(pkt, 0, D_700036A0, *(float *)(blk + 4), f, 1.0f, 0.1f);
            func_001CFBE0(h, 1, D_overlay_AREA08_00827DD0 + k * 0x90, pkt, 0);
            *(float *)(blk + 4) += 0.008f;
            if (!(*(float *)(blk + 4) <= 2.0f)) {
                *(float *)(blk + 4) -= 1.0f;
            }
            blk += 4;
        }
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
