// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA13 overlay, runtime 0x00827C30 (splat/link name 00827BF0; overlay code
//  is linked 0x40 below where it runs), 0x19C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: step 0 of 0x827150 (0x82D190[0]): at count 0x1DF
//  func_001EFD20(0x80000054 / 6) at 0x82D1B0[0], +0x28 += 1, markers left -=
//  1, func_001F02C0(.., 0x8D3, 500), func_001B1E20(8, 100); at 0x1F3 ORs 0x20
//  into D_008107F4 and calls func_001EFD20(0x80000041) and func_001F02C0(..,
//  0x44D, 100) at (743, 230, 1265); counts 1 / 100 / 200 / 300 / 400 set the
//  blink bit to 5 / 4 / 3 / 2 / 1; limit 0x30C.
typedef struct { float x, y, z, w; } Vec4;
extern int *D_00275CA8;
extern Vec4 D_overlay_AREA13_0082D1B0[];
extern void func_001EFD20(unsigned int msg, void *pos);
extern void func_001F02C0(void *pos, int id, float vol);
extern void func_001B1E20(int a, int b);

extern unsigned char D_008107F4[];
void func_overlay_AREA13_00827BF0(unsigned char *self) {
    Vec4 v;
    if (D_00275CA8[1] == 0x1DF) {
        func_001EFD20(0x80000054, D_overlay_AREA13_0082D1B0);
        func_001EFD20(6, D_overlay_AREA13_0082D1B0);
        *(short *)(self + 0x28) += 1;
        D_00275CA8[3]--;
        func_001F02C0(D_overlay_AREA13_0082D1B0, 0x8D3, 500.0f);
        func_001B1E20(8, 0x64);
    }
    if (D_00275CA8[1] == 0x1F3) {
        D_008107F4[0] |= 0x20;
        v.x = 743.0f;
        v.y = 230.0f;
        v.z = 1265.0f;
        v.w = 1.0f;
        func_001EFD20(0x80000041, &v);
        func_001F02C0(&v, 0x44D, 100.0f);
    }
    switch (D_00275CA8[1]) {
    case 1:
        D_00275CA8[6] = 5;
        break;
    case 0x64:
        D_00275CA8[6] = 4;
        break;
    case 0xC8:
        D_00275CA8[6] = 3;
        break;
    case 0x12C:
        D_00275CA8[6] = 2;
        break;
    case 0x190:
        D_00275CA8[6] = 1;
        break;
    }
    D_00275CA8[2] = 0x30C;
}
