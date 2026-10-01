// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00826840 (splat/link name 00826800; overlay code
//  is linked 0x40 below where it runs), 0xA8 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Covers the splat pieces 00826800, 00826840 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called from 0x8266F0. +5 0: when D_0081080E == 1,
//  func_0019C6F0(0x1C, 0) and script 0x82BEC0 (+5 1); +5 1: at the script end
//  +5 = 0, func_001FAE70(0), D_0081080E = 0x11.
extern unsigned char D_0081080E[];
extern char D_overlay_AREA21_0082BEC0[];
extern void func_0019C6F0(int id, int a1);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001FAE70(int a);

void func_overlay_AREA21_00826800(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (D_0081080E[0] == 1) {
            func_0019C6F0(0x1C, 0);
            func_001BA1A0(talk, D_overlay_AREA21_0082BEC0);
            self[5] = 1;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            self[5] = 0;
            func_001FAE70(0);
            D_0081080E[0] = 0x11;
        }
        break;
    }
}
