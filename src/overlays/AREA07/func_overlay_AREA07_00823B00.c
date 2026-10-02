// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA07 overlay, runtime 0x00823B40 (splat/link name 00823B00; overlay code
//  is linked 0x40 below where it runs), 0xDC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA07; lane OVLC).
// Covers the splat pieces 00823B00, 00823B40 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called from 0x823990. +5 0: on +0xB bit 2, func_001B6F00(self, (0,
//  -6, 0, 1), pi) and script 0x8264B0 on +0x1F0, +5 1. +5 1: at the script
//  end func_001C4760(0x11, 1) and D_008107ED = 0xFF (counter 0x15).
extern float D_700038A0[4];
extern unsigned char D_008107ED;
extern char D_overlay_AREA07_008264B0[];
extern void func_001B6F00(unsigned char *self, void *pos, float yaw);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C4760(int id, int n);

void func_overlay_AREA07_00823B00(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (self[0xB] & 4) {
            D_700038A0[0] = 0.0f;
            D_700038A0[1] = -6.0f;
            D_700038A0[2] = 0.0f;
            D_700038A0[3] = 1.0f;
            func_001B6F00(self, D_700038A0, 3.1415927f);
            func_001BA1A0(talk, D_overlay_AREA07_008264B0);
            self[5] = 1;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            func_001C4760(0x11, 1);
            D_008107ED = 0xFF;
        }
        break;
    }
}
