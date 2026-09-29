// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00825320 (splat/link name 008252E0; overlay code is
// linked 0x40 below where it runs), 0x110 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Role: state 0: func_001B0FD0, func_001C6380, +0x1F0 = 0.01 + (-0.002 + 0.004
//  * rand / 2^31). State 1: +0xC4 = func_001B1470(+0xC4 + +0x1F0),
//  func_001C6380, func_001A2370(self, self + 0xD0), func_001B17A0, +0x4C
//  method. States 2/3 func_001AFC10.
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_00122BB8(void);
extern float func_001B1470(float a);
extern void func_001A2370(unsigned char *self, void *p);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA15_008252E0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        func_001B0FD0(self);
        func_001C6380(self);
        *(float *)(self + 0x1F0) =
            0.01f + (-0.002f + 0.004f * (4.656612873e-10f * (float)func_00122BB8()));
        break;
    case 1:
        *(float *)(self + 0xC4) =
            func_001B1470(*(float *)(self + 0xC4) + *(float *)(self + 0x1F0));
        func_001C6380(self);
        func_001A2370(self, self + 0xD0);
        func_001B17A0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
