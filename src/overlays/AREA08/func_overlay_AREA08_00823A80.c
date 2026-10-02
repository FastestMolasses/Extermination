// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA08 overlay, runtime 0x00823AC0 (splat/link name 00823A80; overlay code
//  is linked 0x40 below where it runs), 0x730 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA08; lane OVLC).
// Role: sub 2 placement [0]: an emitter. State 0: +0x1F0 = 2 + rand % 2,
//  +0x1F4 = 12 + rand % 12, state 1; state 3 at once when D_008107F0 == 0xFF.
//  State 1: when +0x1F0 counts below 0 (reset to 2 + rand % 2), between one
//  of two point pairs (rand % 2) two random points D_700036A0 / D_700036B0,
//  effect 0x8000003B (func_001EFEB0) on the axis D_700036E0 through their
//  midpoint, the effect's +5 = 1, +0x1F0 = 12, +0x1F4 = half their distance,
//  +0x1F8 = 0.4. When +0x1F4 counts below 0 (reset to 12 + rand % 12), effect
//  0x80000042 at a random point of a pair with yaw -0.34906587 and a random
//  roll. Every 100th D_70003B68 frame func_001FB9F0(0x41D / 0x41E / 0x41F,
//  0x1000 x3) by rand % 4 (3: none). States 2 / 3 func_001AFC10.
extern unsigned char D_008107F0;
extern float D_700036A0[];
extern float D_700036B0[];
extern float D_700036E0[];
extern float D_700038A0[];
extern float D_700038B0[];
extern float D_700038C0[];
extern float D_700038D0[];
extern int D_70003B68[];
extern int func_00122BB8(void);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_001028E8(void *dst, void *a, void *b);
extern void func_001028B8(void *dst, void *a, void *b);
extern float func_0011E748(float a);
extern void func_00102760(void *dst, void *src);
extern void func_001CD390(void *dst, void *v);
extern void func_00102918(void *dst, void *src, void *v);
extern void func_001029C0(void *m);
extern void func_00102B08(void *dst, void *src, float a);
extern void func_00102BB0(void *dst, void *src, float a);
extern unsigned char *func_001EFEB0(int id, void *m);
extern void func_001FB9F0(int id, int a, int b, int c);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA08_00823A80(unsigned char *self) {
    int i;
    unsigned char *blk = self + 0x1F0;
    float *m;
    unsigned char *o;
    switch (self[4]) {
    case 0:
        *(int *)blk = func_00122BB8() % 2 + 2;
        *(int *)(blk + 4) = func_00122BB8() % 12 + 12;
        self[4] = 1;
        if (D_008107F0 == 0xFF) {
            self[4] = 3;
            break;
        }
    case 1:
        *(int *)blk -= 1;
        if (*(int *)blk < 0) {
            *(int *)blk = func_00122BB8() % 2 + 2;
            switch (func_00122BB8() % 2) {
            case 0:
                D_700038A0[0] = 340.0f;
                D_700038A0[1] = 222.0f;
                D_700038A0[2] = 160.0f;
                D_700038A0[3] = 1.0f;
                D_700038B0[0] = 360.0f;
                D_700038B0[1] = 222.0f;
                D_700038B0[2] = 300.0f;
                D_700038B0[3] = 1.0f;
                break;
            case 1:
                D_700038A0[0] = 240.0f;
                D_700038A0[1] = 222.0f;
                D_700038A0[2] = 155.0f;
                D_700038A0[3] = 1.0f;
                D_700038B0[0] = 360.0f;
                D_700038B0[1] = 222.0f;
                D_700038B0[2] = 175.0f;
                D_700038B0[3] = 1.0f;
                break;
            }
            for (i = 0, m = D_700036A0; i < 2; i++) {
                D_700038C0[0] = (float)func_00122BB8() / 2147483648.0f;
                D_700038C0[1] = (float)func_00122BB8() / 2147483648.0f;
                D_700038C0[2] = (float)func_00122BB8() / 2147483648.0f;
                D_700038C0[3] = 1.0f;
                func_001028D0(m, D_700038B0, D_700038A0);
                func_001028E8(m, m, D_700038C0);
                func_001028B8(m, m, D_700038A0);
                m += 4;
            }
            func_001028D0(D_700038A0, D_700036B0, D_700036A0);
            *(float *)0x70003A20 = func_0011E748(D_700038A0[0] * D_700038A0[0] +
                                                 D_700038A0[1] * D_700038A0[1] +
                                                 D_700038A0[2] * D_700038A0[2]);
            func_00102760(D_700038A0, D_700038A0);
            func_001CD390(D_700036E0, D_700038A0);
            func_00102918(D_700036E0, D_700036E0, D_700036A0);
            o = func_001EFEB0(0x8000003B, D_700036E0);
            if (o != 0) {
                o[5] = 1;
                *(int *)(o + 0x1F0) = 12;
                *(float *)(o + 0x1F4) = *(float *)0x70003A20;
                *(float *)(o + 0x1F8) = 0.4f;
            }
        }
        *(int *)(blk + 4) -= 1;
        if (*(int *)(blk + 4) < 0) {
            *(int *)(blk + 4) = func_00122BB8() % 12 + 12;
            switch (func_00122BB8() % 2) {
            case 0:
                D_700038A0[0] = 340.0f;
                D_700038A0[1] = 222.0f;
                D_700038A0[2] = 160.0f;
                D_700038A0[3] = 1.0f;
                D_700038B0[0] = 360.0f;
                D_700038B0[1] = 222.0f;
                D_700038B0[2] = 300.0f;
                D_700038B0[3] = 1.0f;
                break;
            case 1:
                D_700038A0[0] = 240.0f;
                D_700038A0[1] = 222.0f;
                D_700038A0[2] = 155.0f;
                D_700038A0[3] = 1.0f;
                D_700038B0[0] = 360.0f;
                D_700038B0[1] = 222.0f;
                D_700038B0[2] = 175.0f;
                D_700038B0[3] = 1.0f;
                break;
            }
            D_700038C0[0] = (float)func_00122BB8() / 2147483648.0f;
            D_700038C0[1] = (float)func_00122BB8() / 2147483648.0f;
            D_700038C0[2] = (float)func_00122BB8() / 2147483648.0f;
            D_700038C0[3] = 1.0f;
            func_001028D0(D_700038D0, D_700038B0, D_700038A0);
            func_001028E8(D_700038D0, D_700038D0, D_700038C0);
            func_001028B8(D_700038D0, D_700038D0, D_700038A0);
            func_001029C0(D_700036A0);
            func_00102B08(D_700036A0, D_700036A0, -0.34906587f);
            func_00102BB0(D_700036A0, D_700036A0, 6.2831855f * ((float)func_00122BB8() / 2147483648.0f) - 3.1415927f);
            func_00102918(D_700036A0, D_700036A0, D_700038D0);
            func_001EFEB0(0x80000042, D_700036A0);
        }
        if (D_70003B68[0] % 100 == 0) {
            switch (func_00122BB8() % 4) {
            case 0:
                func_001FB9F0(0x41D, 0x1000, 0x1000, 0x1000);
                break;
            case 1:
                func_001FB9F0(0x41E, 0x1000, 0x1000, 0x1000);
                break;
            case 2:
                func_001FB9F0(0x41F, 0x1000, 0x1000, 0x1000);
                break;
            case 3:
                break;
            }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
