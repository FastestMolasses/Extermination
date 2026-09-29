// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00826850 (splat/link name 00826810; overlay code is
// linked 0x40 below where it runs), 0x120 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Role: state 0: func_001B0FD0, func_001CA5F0(self, 0xC) for +0xD 0xD,
//  func_001C6380. State 1: +0xD 0xC goes to state 3 once D_008107FB >= 3, else
//  func_001C6380, func_001B17A0, +0x4C; other +0xD animate (func_001C6380,
//  func_001B1B70, +0x4C) only once D_008107FB >= 3. States 2/3 func_001AFC10.
extern unsigned char D_008107FB;
extern int func_001B0FD0(unsigned char *self);
extern void func_001CA5F0(unsigned char *self, int a1);
extern void func_001C6380(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA15_00826810(unsigned char *self) {
    switch (self[4]) {
    case 0:
        func_001B0FD0(self);
        if (self[0xD] == 0xD) {
            func_001CA5F0(self, 0xC);
        }
        func_001C6380(self);
        break;
    case 1:
        if (self[0xD] == 0xC) {
            if (D_008107FB >= 3) {
                self[4] = 3;
            } else {
                func_001C6380(self);
                func_001B17A0(self);
                (*(void (**)(unsigned char *))(self + 0x4C))(self);
            }
        } else if (D_008107FB >= 3) {
            func_001C6380(self);
            func_001B1B70(self);
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
