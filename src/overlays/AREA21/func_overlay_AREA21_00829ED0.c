// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00829F10 (splat/link name 00829ED0; overlay code
//  is linked 0x40 below where it runs), 0x2C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: called from 0x8297C0: +0x240 = 0, +0x244 = func_00122BB8().
extern int func_00122BB8(void);

void func_overlay_AREA21_00829ED0(unsigned char *self) {
    unsigned char *w = self + 0x1F0;
    *(int *)(w + 0x50) = 0;
    *(int *)(w + 0x54) = func_00122BB8();
}
