// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA02 overlay, runtime 0x00824AC0 (splat/link name 00824A80; overlay
// code is linked 0x40 below where it runs), 0x178 bytes. Byte-identical
// (tools/overlay/overlay_match.py check AREA02; lane A02C).
// Covers the splat pieces 00824A80, 00824AC0 (the later piece is absorbed at
// link time, tools/overlay/fill_overlay.py).
// Role: when D_70003B68 % 8 == 0, spawns four func_001EFD20(0x80000030,
//  D_700038A0) objects: position (10, 0, -20 + rand % 80, 1) through
//  func_001026A0 with bone +0x110[0] + 0x90, then x += 40; +0xC0 =
//  (rand % 150 + 15) and +0xC4 = rand % 360, both degrees to radians; +0xC8 =
//  0.
extern int D_70003B68;
extern float D_700038A0[4];
extern char *func_001EFD20(int id, void *a);
extern int func_00122BB8(void);
extern void func_001026A0(void *dst, void *a, void *b);

void func_overlay_AREA02_00824A80(unsigned char *self) {
    int i;
    char *e;

    if (D_70003B68 % 8 != 0) {
        return;
    }
    for (i = 0; i < 4; i++) {
        e = func_001EFD20(0x80000030, D_700038A0);
        if (e == 0) {
            continue;
        }
        *(float *)(e + 0xB0) = 10.0f;
        *(float *)(e + 0xB4) = 0.0f;
        *(float *)(e + 0xB8) = -20.0f + (float)(func_00122BB8() % 80);
        *(float *)(e + 0xBC) = 1.0f;
        func_001026A0(e + 0xB0, *(char **)(self + 0x110) + 0x90, e + 0xB0);
        *(float *)(e + 0xB0) += 40.0f;
        *(float *)(e + 0xC0) = 3.1415927f * (float)(func_00122BB8() % 150 + 15) / 180.0f;
        *(float *)(e + 0xC4) = 3.1415927f * (float)(func_00122BB8() % 360) / 180.0f;
        *(int *)(e + 0xC8) = 0;
    }
}
