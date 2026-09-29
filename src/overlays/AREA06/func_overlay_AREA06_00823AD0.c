// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA06 overlay, runtime 0x00823B10 (splat/link name 00823AD0; overlay code is
// linked 0x40 below where it runs), 0x40 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA06; lane A06C).
// Role: (self, arg): when bit 1 of arg[0] is clear, func_001EFE00(0x80000044,
// arg) and self +0x1F0 = 60. func_overlay_AREA06_00823B10 stores the value
// 0x823B10 at +0x34 of its record, which is this function's runtime address.
// Replaces the earlier hybrid asm body (same bytes).
extern int func_001EFE00(int a, unsigned char *b);

void func_overlay_AREA06_00823AD0(unsigned char *self, unsigned char *a1) {
    if (!(a1[0] & 2)) {
        func_001EFE00(0x80000044, a1);
        *(int *)(self + 0x1F0) = 0x3C;
    }
}
