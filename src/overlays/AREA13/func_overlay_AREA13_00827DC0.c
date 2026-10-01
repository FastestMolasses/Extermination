// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA13 overlay, runtime 0x00827E00 (splat/link name 00827DC0; overlay code
//  is linked 0x40 below where it runs), 0x120 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: step 2 of 0x827150: at counts 0 / 0x5A / 0x96
//  func_001EFD20(0x8000004A / 0x8000002F) at 0x82D1B0[1..3], +0x28 += 1,
//  markers left -= 1, func_001F02C0(+0xB0, 0x44B, 500); limit 0x12C.
typedef struct { float x, y, z, w; } Vec4;
extern int *D_00275CA8;
extern Vec4 D_overlay_AREA13_0082D1B0[];
extern void func_001EFD20(unsigned int msg, void *pos);
extern void func_001F02C0(void *pos, int id, float vol);

void func_overlay_AREA13_00827DC0(unsigned char *self) {
    int n;
    Vec4 *p;
    switch (D_00275CA8[1]) {
    case 0:
        n = 1;
        break;
    case 0x5A:
        n = 2;
        break;
    case 0x96:
        n = 3;
        break;
    default:
        n = -1;
        break;
    }
    if (n > 0) {
        *(float *)0x700038A0 = -1.5707964f;
        *(float *)0x700038A4 = 0.0f;
        *(float *)0x700038A8 = 0.0f;
        *(float *)0x700038AC = 1.0f;
        p = &D_overlay_AREA13_0082D1B0[n];
        func_001EFD20(0x8000004A, p);
        func_001EFD20(0x8000002F, p);
        *(short *)(self + 0x28) += 1;
        D_00275CA8[3]--;
        func_001F02C0(self + 0xB0, 0x44B, 500.0f);
    }
    D_00275CA8[2] = 0x12C;
}
