// NEARMISS func_overlay_AREA13_00827F50 (98.46%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA13 overlay, runtime 0x00827F90 (splat/link name 00827F50; overlay code
//  is linked 0x40 below where it runs), 0x548 bytes.
// Role (read from the instructions): step 4 of 0x827150. Phase 0 (block [4]):
//  every 45 frames a random point up the matrix at (*(D_00275B40 + 4)) + 0x90
//  gets func_001EFD20 effects (a random one of 0 / 1 / 2, then 2, 2,
//  0x8000004A, 0x8000002F, and 3 at count 0) and func_001F02C0(.., 0x8D6,
//  200); every 20 frames a second random point is computed and not used; when
//  +0x3C <= 10 and the player is in the area 0x82D330 below y 210: D_008102BF
//  = 0xB, D_008102B0 |= 2, D_008104D4 = D_008104D0; when +0x3C <= 4: phase 1,
//  func_001F02C0(.., 0x8D5, 500), D_00810833 = 0xFF. Phase 1: at block [5] 0
//  five func_001EFD20(4, ..) at points stepped from (795, 180, 1130) toward
//  (680, 170, 1015); after 30 frames +5 += 1. Always func_001C64F0(self, 1.0)
//  and limit 0xFFFF.
// Divergence: the original counts the five-spark loop down (the counter
//  starts at 4 and is tested for the last pass before the decrement), and it
//  keeps self in s1 where mwcc 2.3.3 uses s0; the rest of the body and every
//  call match. for / while / do-while spellings, count-up and count-down, a
//  pointer walk and declaration orders were tried.
extern int *D_00275CA8;
extern float D_70003A20[];
extern char *D_00275B40;
extern float D_700038A0[4];
extern float D_700038B0[4];
extern float D_00810350[];
extern char D_overlay_AREA13_0082D330[];
extern int func_00122BB8(void);
extern void func_001026A0(void *dst, void *m, void *v);
extern void func_001EFD20(unsigned int msg, void *pos);
extern void func_001F02C0(void *pos, int id, float vol);
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00102850(void *dst, void *src, float k);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001C64F0(unsigned char *self, float step);

void func_overlay_AREA13_00827F50(unsigned char *self) {
    int i;
    float t;
    switch (D_00275CA8[4]) {
    case 0:
        if (D_00275CA8[1] % 45 == 0) {
            D_70003A20[0] = (float)func_00122BB8() / 2147483648.0f;
            t = *(float *)0x70003A20;
            D_700038A0[0] = 15.0f - 20.0f * t;
            D_700038A0[1] = 210.0f * t;
            D_700038A0[2] = -15.0f - 20.0f * t;
            D_700038A0[3] = 1.0f;
            func_001026A0(D_700038A0, *(char **)(D_00275B40 + 4) + 0x90, D_700038A0);
            D_700038B0[0] = -1.5707964f;
            D_700038B0[1] = 0.0f;
            D_700038B0[2] = 0.0f;
            D_700038B0[3] = 1.0f;
            switch (func_00122BB8() % 3) {
            case 0:
                func_001EFD20(0, D_700038A0);
                break;
            case 1:
                func_001EFD20(1, D_700038A0);
                break;
            case 2:
                func_001EFD20(2, D_700038A0);
                break;
            }
            func_001EFD20(2, D_700038A0);
            func_001EFD20(2, D_700038A0);
            func_001EFD20(0x8000004A, D_700038A0);
            func_001EFD20(0x8000002F, D_700038A0);
            if (D_00275CA8[1] == 0) {
                func_001EFD20(3, D_700038A0);
            }
            func_001F02C0(self + 0xB0, 0x8D6, 200.0f);
        }
        if (D_00275CA8[1] % 20 == 0) {
            D_70003A20[0] = (float)func_00122BB8() / 2147483648.0f;
            t = *(float *)0x70003A20;
            D_700038A0[0] = 15.0f - 20.0f * t;
            D_700038A0[1] = 210.0f * t;
            D_700038A0[2] = -15.0f - 20.0f * t;
            D_700038A0[3] = 1.0f;
            func_001026A0(D_700038A0, *(char **)(D_00275B40 + 4) + 0x90, D_700038A0);
            D_700038B0[0] = 1.5707964f;
            D_700038B0[1] = 0.0f;
            D_700038B0[2] = 0.0f;
            D_700038B0[3] = 1.0f;
        }
        if (*(float *)(self + 0x3C) <= 10.0f &&
            func_001B1EA0(0, D_00810350, D_overlay_AREA13_0082D330, 8) != 0 &&
            *(float *)0x810354 < 210.0f) {
            *(unsigned char *)0x8102BF = 0xB;
            *(unsigned char *)0x8102B0 |= 2;
            *(float *)0x8104D4 = *(float *)0x8104D0;
        }
        if (*(float *)(self + 0x3C) <= 4.0f) {
            D_00275CA8[4]++;
            func_001F02C0(self + 0xB0, 0x8D5, 500.0f);
            *(unsigned char *)0x810833 = 0xFF;
        }
        break;
    case 1:
        if (D_00275CA8[5] == 0) {
            D_700038A0[0] = 795.0f;
            D_700038A0[1] = 180.0f;
            D_700038A0[2] = 1130.0f;
            D_700038A0[3] = 1.0f;
            D_700038B0[0] = 680.0f;
            D_700038B0[1] = 170.0f;
            D_700038B0[2] = 1015.0f;
            D_700038B0[3] = 1.0f;
            func_001028D0(D_700038B0, D_700038B0, D_700038A0);
            func_00102850(D_700038B0, D_700038B0, 4.0f);
            for (i = 4; i >= 0; i--) {
                func_001EFD20(4, D_700038A0);
                func_001028B8(D_700038A0, D_700038A0, D_700038B0);
            }
        }
        D_00275CA8[5]++;
        if (D_00275CA8[5] > 0x1E) {
            self[5]++;
        }
        break;
    }
    func_001C64F0(self, 1.0f);
    D_00275CA8[2] = 0xFFFF;
}
