// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x00824E40 (splat/link name 00824E00; overlay code
//  is linked 0x40 below where it runs), 0x29C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: sub 0 placement [40]. State 0: six points +0x1F0..+0x240 (fixed local
//  coordinates) through +0xD0. State 1: flag 0x2F gives the +0x114 object's
//  +0x8A = 0x7D00 and state 2; func_001B1B70 and the +0x4C method unless
//  D_70003B92 and D_00810787 are both set; func_001F5940(1, point, 0) for the
//  six points. State 2: the +0x4C method; state 3 func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
#define F(o) (*(float *)(self + (o)))
extern unsigned char D_00810787;
extern unsigned char D_70003B92[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001026A0(void *dst, void *m, void *v);
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B1B70(unsigned char *self);
extern void func_001F5940(int id, float *v, int a2);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA17_00824E00(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        F(0x1F0) = -88.567f;
        F(0x1F4) = 161.814f;
        F(0x1F8) = 60.85f;
        F(0x200) = -93.098f;
        F(0x204) = 161.814f;
        F(0x208) = 53.966f;
        F(0x210) = -55.047f;
        F(0x214) = 127.77f;
        F(0x218) = 28.273f;
        F(0x220) = -49.292f;
        F(0x224) = 127.77f;
        F(0x228) = 37.225f;
        F(0x230) = -17.464f;
        F(0x234) = 96.156f;
        F(0x238) = 17.702f;
        F(0x240) = -24.076f;
        F(0x244) = 96.156f;
        F(0x248) = 7.621f;
        F(0x1FC) = F(0x20C) = F(0x21C) = F(0x22C) = F(0x23C) = F(0x24C) = 1.0f;
        func_001026A0(self + 0x1F0, self + 0xD0, self + 0x1F0);
        func_001026A0(self + 0x200, self + 0xD0, self + 0x200);
        func_001026A0(self + 0x210, self + 0xD0, self + 0x210);
        func_001026A0(self + 0x220, self + 0xD0, self + 0x220);
        func_001026A0(self + 0x230, self + 0xD0, self + 0x230);
        func_001026A0(self + 0x240, self + 0xD0, self + 0x240);
        break;
    case 1:
        if (func_001BA1C0(self, 0x2F) != 0) {
            *(short *)(*(unsigned char **)(self + 0x114) + 0x8A) = 0x7D00;
            func_001C6380(self);
            self[4] = 2;
        }
        if (D_70003B92[0] == 0 || D_00810787 == 0) {
            func_001B1B70(self);
            (*(ActorFn *)(self + 0x4C))(self);
        }
        func_001F5940(1, (float *)(self + 0x1F0), 0);
        func_001F5940(1, (float *)(self + 0x200), 0);
        func_001F5940(1, (float *)(self + 0x210), 0);
        func_001F5940(1, (float *)(self + 0x220), 0);
        func_001F5940(1, (float *)(self + 0x230), 0);
        func_001F5940(1, (float *)(self + 0x240), 0);
        break;
    case 2:
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
