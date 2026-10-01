// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x008257C0 (splat/link name 00825780; overlay code
//  is linked 0x40 below where it runs), 0x164 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: sub 0 placement [7]. State 0: when func_001B0F60(self, 0xB) returns
//  0: state 1, +8 = 3, +0x30 = 0x82C6D0, +0 = 2 when func_001BA1C0(self,
//  0x1E) is set, else 1. State 1: 0x825AB0 while D_008107F6 == 0, else
//  0x825930; func_001F5940(7, position, 0) while D_008107F6 < 3; then
//  func_001C68C0, func_001B17A0 and the +0x4C method. States 2/3
//  func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_008107F6;
extern float D_700038A0[];
extern char D_overlay_AREA19_0082C6D0[];
extern int func_001B0F60(unsigned char *self, int a1);
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001F5940(int kind, void *pos, int a2);
extern void func_001C68C0(unsigned char *self);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);
extern void func_overlay_AREA19_00825930(unsigned char *self);
extern void func_overlay_AREA19_00825AB0(unsigned char *self);

void func_overlay_AREA19_00825780(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B0F60(self, 0xB) == 0) {
            self[4] = 1;
            self[8] = 3;
            *(void **)(self + 0x30) = D_overlay_AREA19_0082C6D0;
            if (func_001BA1C0(self, 0x1E) != 0) {
                self[0] = 2;
            } else {
                self[0] = 1;
            }
        }
        break;
    case 1:
        if (D_008107F6 == 0) {
            func_overlay_AREA19_00825AB0(self);
        } else {
            func_overlay_AREA19_00825930(self);
        }
        if (D_008107F6 < 3) {
            *(float *)0x700038A0 = *(float *)(self + 0xB0);
            *(float *)0x700038A4 = *(float *)(self + 0xB4);
            *(float *)0x700038A8 = *(float *)(self + 0xB8);
            *(float *)0x700038AC = 1.0f;
            func_001F5940(7, D_700038A0, 0);
        }
        func_001C68C0(self);
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
