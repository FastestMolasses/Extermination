// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x00824CF0 (splat/link name 00824CB0; overlay code
//  is linked 0x40 below where it runs), 0x150 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: sub 0 placement [42]. State 0: +0x1F0 = (0.4, 14.9, 0.25, 1) through
//  +0xD0. State 1: with D_00810806 == 1 +0x28 counts frames and
//  func_001F5940(7, +0x1F0, 0) runs for frames 567..745 and 1106..1124; then
//  func_001B17A0 and the +0x4C method. States 2 / 3 func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
#define S16(o) (*(short *)(self + (o)))
extern unsigned char D_00810806;
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001026A0(void *dst, void *m, void *v);
extern void func_001F5940(int id, float *v, int a2);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA17_00824CB0(unsigned char *self) {
    short n;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        *(float *)(self + 0x1F0) = 0.4f;
        *(float *)(self + 0x1F4) = 14.9f;
        *(float *)(self + 0x1F8) = 0.25f;
        *(float *)(self + 0x1FC) = 1.0f;
        func_001026A0(self + 0x1F0, self + 0xD0, self + 0x1F0);
        break;
    case 1:
        if (D_00810806 == 1) {
            S16(0x28)++;
            n = S16(0x28);
            if (n > 566 && n <= 745) {
                func_001F5940(7, (float *)(self + 0x1F0), 0);
            } else if (n > 1105 && n <= 1124) {
                func_001F5940(7, (float *)(self + 0x1F0), 0);
            }
        }
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
