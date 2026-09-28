// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00823A90 (splat/link name 00823A50;
// overlay code is linked 0x40 below where it runs), 0x80 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: state 1 sends func_001EFD20(0x8000001B, self + 0x100) on every
// 50th value of D_70003B68.
extern int D_70003B68;
extern void func_001EFD20(int code, void *a);

void func_overlay_AREA04_00823A50(unsigned char *self) {
    switch (self[4]) {
    case 0:
        break;
    case 1:
        if (D_70003B68 % 50 == 0) {
            func_001EFD20(0x8000001B, self + 0x100);
        }
        break;
    case 2:
    case 3:
        break;
    }
}
