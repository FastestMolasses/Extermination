// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00823920 (splat/link name 008238E0;
// overlay code is linked 0x40 below where it runs), 0x7C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: state 1 sends func_001EFEB0(0, self + 0xD0) on every 40th value
// of D_70003B68.
extern int D_70003B68;
extern void func_001EFEB0(int id, void *a);

void func_overlay_AREA04_008238E0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        break;
    case 1:
        if (D_70003B68 % 40 == 0) {
            func_001EFEB0(0, self + 0xD0);
        }
        break;
    case 2:
    case 3:
        break;
    }
}
