// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00826010 (splat/link name 00825FD0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: called from 0x825BE0 and 0x825D30; every sixth frame (random 2..5
//  frame skip) probes toward the target at +0x1C and emits event 0x80000003
//  on a hit.
// Covers the splat pieces 00825FD0, 00826010 (the later piece
// is absorbed at link time, tools/overlay/fill_overlay.py).
typedef struct { float x, y, z, w; } Vec4;
extern float D_700038A0[4];
extern float D_700031B0[4];
extern int func_00122BB8(void);
extern void func_00103230(void *a, void *b, float angle);
extern int func_001FBD50(void *a0, int a1, int a2, float f12);
extern void func_001B0CD0(void *a0, int a1);
extern int func_0019A570(void *a, void *b, int c, int d);
extern void func_001031E0(void *dst, void *src);
extern float func_001B1470(float a);
extern void func_001EFD90(int id, void *a, void *b);

void func_overlay_AREA01_00825FD0(unsigned char *self) {
    unsigned char *pl = *(unsigned char **)(self + 0x1C);
    Vec4 rot;
    int x;
    *(short *)(self + 0x2A) += 1;
    if (*(short *)(self + 0x2A) % 6 == 0) {
        if (*(short *)(self + 0x2A) > 0xB4) /* spelled > 0xB4 for the compare register (idiom-28) */ {
            x = func_00122BB8() >> 16;
            x *= 4;
            x >>= 15;
            self[0x240] = x + 2;
            *(short *)(self + 0x2A) = 0;
        }
        if (self[0x240] > 0) {
            self[0x240]--;
            return;
        }
        func_00103230(D_700038A0, pl + 0xC0, 300.0f);
        *(volatile float *)0x700038A0 += *(float *)(pl + 0xB0);
        *(volatile float *)0x700038A4 += *(float *)(pl + 0xB4);
        *(volatile float *)0x700038A8 += *(float *)(pl + 0xB8);
        func_001FBD50(self, 0x164, 0, 800.0f);
        func_001B0CD0(pl, 0);
        if (func_0019A570(pl + 0xB0, D_700038A0, 4, 0x20) != 0) {
            func_001031E0(D_700038A0, D_700031B0);
            rot.x = 0.0f;
            rot.y = func_001B1470(3.14159265f + *(float *)(self + 0xC4));
            rot.z = 0.0f;
            func_001EFD90(0x80000003, D_700038A0, &rot);
        }
    }
}
