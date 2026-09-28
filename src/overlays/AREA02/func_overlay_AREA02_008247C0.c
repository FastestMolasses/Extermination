// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 2
// AREA02 overlay, runtime 0x00824800 (splat/link name 008247C0; overlay
// code is linked 0x40 below where it runs), 0x104 bytes. Byte-identical
// (tools/overlay/overlay_match.py check AREA02; lane A02C).
// Covers the splat pieces 008247C0, 00824800 (the later piece is absorbed at
// link time, tools/overlay/fill_overlay.py).
// Role: model setup (the func_001B0DC0 shape): func_001CA6E0(self,
//  func_001C6120(D_0028A59C, kind)), +0x40 = D_0028A6E8, bone count +0xC =
//  func_001C6150(+0x44); above D_00275BCC it sets +4 = 3 and returns 0.
//  Otherwise one func_001AF780 result per bone goes to +0x110[i], +9 =
//  count, func_001CB5B0(count); kind 7 calls func_001C63E0(self, 0), kind 8
//  func_001C63E0(self, 1); returns 1.
extern int D_0028A59C;
extern int D_0028A6E8;
extern short D_00275BCC;
extern char *func_001C6120(int a0, int a1);
extern void func_001CA6E0(unsigned char *self, char *model);
extern unsigned char func_001C6150(int a0);
extern int func_001AF780(void);
extern void func_001CB5B0(int a0);
extern void func_001C63E0(unsigned char *self, short a1);

int func_overlay_AREA02_008247C0(unsigned char *self) {
    int i;
    unsigned char *p;
    unsigned char n;

    func_001CA6E0(self, func_001C6120(D_0028A59C, self[0xD]));
    *(int *)(self + 0x40) = D_0028A6E8;
    self[0xC] = func_001C6150(*(int *)(self + 0x44));
    if (D_00275BCC < (int)self[0xC]) {
        self[4] = 3;
        return 0;
    }
    for (i = 0, p = self; i < (int)(n = self[0xC]); i += 1) {
        *(int *)(p + 0x110) = func_001AF780();
        p += 4;
    }
    self[9] = n;
    func_001CB5B0(self[0xC]);
    if (self[0xD] == 7) {
        func_001C63E0(self, 0);
    } else if (self[0xD] == 8) {
        func_001C63E0(self, 1);
    }
    return 1;
}
