// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA20 overlay, runtime 0x008239B0 (splat/link name 00823970; overlay code
//  is linked 0x40 below where it runs), 0x150 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA20; lane OVLC).
// Role: sub 0 placement [32] (the AREA21 request; counter 0x34). State 0:
//  state 3 when flag 0x34; else the model setup (func_001B10B0 0x5B,
//  func_001C63E0 3, func_001CA6F0 2, func_001BA8E0), state 1, +0 = 1. State
//  1: 0x823B00 while D_0081080C is 0, else 0x823BE0; then a func_001F9100
//  marker at +0xB0, (0, 1, 0, 1), 5.0. States 2 / 3 func_001BA540,
//  func_001AFC10.
extern unsigned char D_0081080C;
extern float D_700038A0[];
extern float D_700038B0[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001CA6F0(unsigned char *self, int a);
extern void func_001BA8E0(unsigned char *self, int a);
extern void func_overlay_AREA20_00823B00(unsigned char *self);
extern void func_overlay_AREA20_00823BE0(unsigned char *self);
extern void func_00102948(void *dst, void *src);
extern void func_001F9100(void *pos, void *a, void *b, float f12);
extern void func_001BA540(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA20_00823970(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x34) != 0) {
            self[4] = 3;
            break;
        }
        func_001B10B0(self, self[0xD], 0x5B);
        func_001C63E0(self, 3);
        func_001CA6F0(self, 2);
        func_001BA8E0(self, self[0xD]);
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        if (D_0081080C == 0) {
            func_overlay_AREA20_00823B00(self);
        } else {
            func_overlay_AREA20_00823BE0(self);
        }
        func_00102948(D_700038A0, self + 0xB0);
        D_700038B0[0] = 0.0f;
        D_700038B0[1] = 1.0f;
        D_700038B0[2] = 0.0f;
        D_700038B0[3] = 1.0f;
        func_001F9100(self + 0xB0, D_700038A0, D_700038B0, 5.0f);
        break;
    case 2:
    case 3:
        func_001BA540(self);
        func_001AFC10(self);
        break;
    }
}
