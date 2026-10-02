// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA08 overlay, runtime 0x00824870 (splat/link name 00824830; overlay code
//  is linked 0x40 below where it runs), 0x17C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA08; lane OVLC).
// Covers the splat pieces 00824830, 00824870 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called from 0x8245E0; the AREA07 0x823C20 C with script 0x8268E0,
//  func_001C4760(0x13, 1), D_00810770 and D_008107F0.
extern float D_700038A0[4];
extern unsigned char D_00810770;
extern unsigned char D_0081077D;
extern unsigned char D_008107F0;
extern char D_overlay_AREA08_008268E0[];
extern void func_001B6F00(unsigned char *self, void *pos, float yaw);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C4760(int id, int n);
extern void func_001FABB0(void);
extern void func_001FB0B0(int id);

void func_overlay_AREA08_00824830(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (self[0xB] & 4) {
            func_001BA1A0(talk, D_overlay_AREA08_008268E0);
            D_700038A0[0] = 0.0f;
            D_700038A0[1] = -6.0f;
            D_700038A0[2] = 0.0f;
            D_700038A0[3] = 1.0f;
            func_001B6F00(self, D_700038A0, 3.1415927f);
            self[5] = 1;
        }
        break;
    case 1:
        if (D_0081077D != 0) {
            if (self[6] == 0) {
            D_700038A0[0] = 0.0f;
            D_700038A0[1] = -8.0f;
            D_700038A0[2] = 0.0f;
            D_700038A0[3] = 1.0f;
            func_001B6F00(self, D_700038A0, 3.1415927f);
            self[6]++;
            }
        }
        if (func_001BA1F0(self) != 0) {
            func_001C4760(0x13, 1);
            func_001C4760(0x14, 1);
            func_001FABB0();
            func_001FB0B0(9);
            D_00810770 = 0xFF;
            D_008107F0 = 0xFF;
            self[5] = 2;
        }
        break;
    case 2:
        break;
    }
}
