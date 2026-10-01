// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x008268F0 (splat/link name 008268B0; overlay code
//  is linked 0x40 below where it runs), 0xA8 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Covers the splat pieces 008268B0, 008268F0 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called from 0x8266F0. +5 0: when D_0081080E == 2, script 0x82C5C0,
//  +5 1, +0x2E = 0; +5 1: at the script end +0x2E = 0xFFFF, D_0081080E = 0xFF,
//  +5 0, func_001FB0B0(0), func_001FAE70(0).
extern unsigned char D_0081080E[];
extern char D_overlay_AREA21_0082C5C0[];
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001FB0B0(int a);
extern void func_001FAE70(int a);

void func_overlay_AREA21_008268B0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (D_0081080E[0] == 2) {
            func_001BA1A0(talk, D_overlay_AREA21_0082C5C0);
            self[5] = 1;
            *(short *)(self + 0x2E) = 0;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            *(unsigned short *)(self + 0x2E) = 0xFFFF;
            D_0081080E[0] = 0xFF;
            self[5] = 0;
            func_001FB0B0(0);
            func_001FAE70(0);
        }
        break;
    }
}
