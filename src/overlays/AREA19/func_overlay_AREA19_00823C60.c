// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00823CA0 (splat/link name 00823C60; overlay code
//  is linked 0x40 below where it runs), 0x6C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: collision method stored by 0x823D10 (+0x34 = 0x823CA0): unless the
//  other object's +0 bit 1 is set, and while func_0021BB00(&D_008102B0) is 0,
//  func_001EFE00(0x80000027, other), other +0xF = 0xC, self +0x204 = 60.
extern void func_001EFE00(unsigned int msg, unsigned char *other);
extern int func_0021BB00(void *p);
extern int D_008102B0;

void func_overlay_AREA19_00823C60(unsigned char *self, unsigned char *other) {
    if (!(other[0] & 2) && func_0021BB00(&D_008102B0) == 0) {
        func_001EFE00(0x80000027, other);
        other[0xF] = 0xC;
        *(int *)(self + 0x204) = 0x3C;
    }
}
