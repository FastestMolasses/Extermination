// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00823C50 (splat/link name 00823C10; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: owner with state byte +4. In state 1, every 20th frame of
//  D_70003B68 it sends func_001EFD90(1, self + 0x100, {pi/2, 0, 0, 1}).
extern int D_70003B68;
extern void func_001EFD90(int id, void *a, void *b);

void func_overlay_AREA00_00823C10(unsigned char *self) {
    float v[4];
    switch (self[4]) {
    case 0:
        break;
    case 1:
        if (D_70003B68 % 20 == 0) {
            v[0] = 1.5707964f;
            v[1] = 0.0f;
            v[2] = 0.0f;
            v[3] = 1.0f;
            func_001EFD90(1, self + 0x100, v);
        }
        break;
    case 2:
    case 3:
        break;
    }
}
