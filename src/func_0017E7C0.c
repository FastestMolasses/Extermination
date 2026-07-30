// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// AI navigation probe: casts a pair of rays (scratchpad work vectors at
// 0x700038A0/0x700038B0, transformed by the entity matrix at arg0+0xD0 into
// 0x700038C0/0x700038D0) against the world via func_0019AFE0, keyed on the
// climb/hang state byte at arg0+0xD. `side` (arg1) mirrors the probe X sign.
// Returns 0 = blocked, 1 = clear (arg0+0x1F1 action id set), 2 = action
// taken (0x1F1 = 1/2/5/6, +0xD updated), 0xA = probe hit but no action.
// Hit-surface type byte = (*(unsigned char **)0x700031D0)[0x1A]
// (0x32/0x3B/0x33/0x3D). The 120..130 x 160..170 box on (+0xB0,+0xB8) with
// D_00810700/1 == 8/3 triggers the special action 5 (stores 123.5/257.5/156.4
// into +0x2E0/4/8 and copies +0xC4 to +0x218).
// The `ni = -5; n5 = (float)ni;` staging on the 12.0f probe is load-bearing
// (idiom-24 generalized to a non-zero constant): it is what makes mwcc emit
// `lui 0xC0A0 / mtc1 $v0,$f13` ahead of `lui 0x4140 / mtc1 $v0,$f12` there.

#define WA0 (*(volatile float *)0x700038A0)
#define WA1 (*(volatile float *)0x700038A4)
#define WA2 (*(volatile float *)0x700038A8)
#define WA3 (*(volatile float *)0x700038AC)
#define WB0 (*(volatile float *)0x700038B0)
#define WB1 (*(volatile float *)0x700038B4)
#define WB2 (*(volatile float *)0x700038B8)
#define WB3 (*(volatile float *)0x700038BC)
#define WD0 (*(volatile float *)0x70003910)
#define WD1 (*(volatile float *)0x70003914)
#define WD2 (*(volatile float *)0x70003918)
#define WD3 (*(volatile float *)0x7000391C)
#define HITOBJ (*(unsigned char **)0x700031D0)

extern float D_700038A0[];
extern float D_700038B0[];
extern float D_700038C0[];
extern float D_700038D0[];
extern float D_700038E0[];
extern float D_700038F0[];
extern float D_70003900[];
extern float D_70003910[];

extern unsigned char D_00810700;
extern unsigned char D_00810701;

extern int func_0017E6E0(unsigned char *p, int side, float a, float b);
extern void func_001026A0(void *dst, void *mat, void *src);
extern void func_001028B8(void *dst, void *a, void *b);
extern int func_0019AFE0(unsigned char *p, void *a, void *b, int mode);
extern int func_001782A0(unsigned char *p);
extern int func_00178440(unsigned char *p);
extern int func_001784E0(void);
extern int func_00178910(unsigned char *p, int a);
extern int func_0017F130(unsigned char *p, int side);

int func_0017E7C0(unsigned char *p, int side)
{
    int found;
    unsigned char v;
    float f;
    int ni;
    float n5;

    found = 0;
    if (*(unsigned char *)(p + 0xD) == 1) {
        if (func_0017E6E0(p, side, 20.0f, -5.0f) == 0
         && func_0017E6E0(p, side, 10.0f, -5.0f) == 0
         && func_0017E6E0(p, side, 0.0f, -5.0f) == 0) {
        if (side == 0) {
            WA0 = -4.5f;
            WB0 = -4.5f;
            WA1 = 20.0f;
            WB1 = 20.0f;
            WA2 = 0.0f;
            WA3 = 1.0f;
            WB2 = 2.0f;
            WB3 = 1.0f;
        } else {
            WA0 = 4.5f;
            WB0 = 4.5f;
            WA1 = 20.0f;
            WB1 = 20.0f;
            WA2 = 0.0f;
            WA3 = 1.0f;
            WB2 = 2.0f;
            WB3 = 1.0f;
        }
        func_001026A0(D_700038C0, p + 0xD0, D_700038A0);
        func_001026A0(D_700038D0, p + 0xD0, D_700038B0);
        if (func_0019AFE0(p, D_700038C0, D_700038D0, 6)) {
            if (HITOBJ[0x1A] == 0x3D) {
                *(unsigned char *)(p + 0x1F1) = 0;
                return 1;
            }
        } else {
            found = 1;
        }
        if (side == 0) {
            WA0 = -9.0f;
            WB0 = -9.0f;
            WA1 = 19.5f;
            WB1 = 19.5f;
            WA2 = 0.0f;
            WA3 = 1.0f;
            WB2 = 10.0f;
            WB3 = 1.0f;
        } else {
            WA0 = 9.0f;
            WB0 = 9.0f;
            WA1 = 19.5f;
            WB1 = 19.5f;
            WA2 = 0.0f;
            WA3 = 1.0f;
            WB2 = 10.0f;
            WB3 = 1.0f;
        }
        func_001026A0(D_700038C0, p + 0xD0, D_700038A0);
        func_001026A0(D_700038D0, p + 0xD0, D_700038B0);
        if (func_0019AFE0(p, D_700038C0, D_700038D0, 7) & 6) {
        v = HITOBJ[0x1A];
        if (v == 0x32) {
            if (func_001782A0(p)) {
                *(unsigned char *)(p + 0x1F1) = 1;
                *(unsigned char *)(p + 0xD) = 0;
                return 2;
            }
            goto ret0;
        }
        if (v == 0x3B) {
            if (func_001782A0(p)) {
                *(unsigned char *)(p + 0x1F1) = 1;
                *(unsigned char *)(p + 0xD) = 1;
                return 2;
            }
            goto ret0;
        }
        if (found == 0) {
            goto ret0;
        }
        if (D_00810700 == 8) {
            if (D_00810701 == 3) {
                f = *(float *)(p + 0xB0);
                if (!(f <= 120.0f) && f < 130.0f) {
                    f = *(float *)(p + 0xB8);
                    if (!(f <= 160.0f) && f < 170.0f) {
                        *(float *)(p + 0x218) = *(float *)(p + 0xC4);
                        *(float *)(p + 0x2E0) = 123.5f;
                        *(float *)(p + 0x2E4) = 257.5f;
                        *(float *)(p + 0x2E8) = 156.4f;
                        *(unsigned char *)(p + 0x1F1) = 5;
                        return 2;
                    }
                }
            }
        }
            return 0xA;
        } else {
            return 0xA;
        }
        }
        return 0;
    }

    if (D_00810700 != 0x11) {
        if (func_001784E0()) {
            *(unsigned char *)(p + 0x1F1) = 2;
            return 2;
        }
    }
    if (side == 0) {
        WA0 = -9.0f;
        WB0 = -9.0f;
        WA1 = 20.0f;
        WB1 = 20.0f;
        WA2 = -2.0f;
        WA3 = 1.0f;
        WB2 = 3.5f;
        WB3 = 1.0f;
    } else {
        WA0 = 9.0f;
        WB0 = 9.0f;
        WA1 = 20.0f;
        WB1 = 20.0f;
        WA2 = -2.0f;
        WA3 = 1.0f;
        WB2 = 3.5f;
        WB3 = 1.0f;
    }
    func_001026A0(D_700038C0, p + 0xD0, D_700038A0);
    func_001026A0(D_700038D0, p + 0xD0, D_700038B0);
    if (func_0019AFE0(p, D_700038C0, D_700038D0, 7) & 6) {
        v = HITOBJ[0x1A];
        if (v == 0x32) {
            if (func_001782A0(p)) {
                *(unsigned char *)(p + 0x1F1) = 1;
                *(unsigned char *)(p + 0xD) = 0;
                return 2;
            }
        } else if (v == 0x3B) {
            if (func_001782A0(p)) {
                *(unsigned char *)(p + 0x1F1) = 1;
                *(unsigned char *)(p + 0xD) = 1;
                return 2;
            }
        } else if (v == 0x33) {
            if (func_00178440(p)) {
                *(unsigned char *)(p + 0x1F1) = 6;
                return 2;
            }
        }
    }
    if (func_0017F130(p, side)) {
        return 0;
    }
    if (func_0017E6E0(p, side, 20.0f, -5.0f)) {
        goto ret0;
    }
    ni = -5;
    n5 = (float)ni;
    if (func_0017E6E0(p, side, 12.0f, n5) == 0
     && func_0017E6E0(p, side, 4.01f, -5.0f) == 0
     && func_0017E6E0(p, side, -0.5f, -5.0f) == 0
     && func_0017E6E0(p, side, 20.0f, 0.0f) == 0
     && func_0017E6E0(p, side, 12.0f, 0.0f) == 0
     && func_0017E6E0(p, side, 4.01f, 0.0f) == 0
     && func_0017E6E0(p, side, -0.5f, 0.0f) == 0) {
    if (side == 0) {
        WA0 = -4.4f;
        WB0 = -4.4f;
        WA1 = 20.0f;
        WB1 = 20.0f;
        WA2 = -2.0f;
        WA3 = 1.0f;
        WB2 = 4.5f;
        WB3 = 1.0f;
    } else {
        WA0 = 4.4f;
        WB0 = 4.4f;
        WA1 = 20.0f;
        WB1 = 20.0f;
        WA2 = -2.0f;
        WA3 = 1.0f;
        WB2 = 4.5f;
        WB3 = 1.0f;
    }
    func_001026A0(D_700038C0, p + 0xD0, D_700038A0);
    func_001026A0(D_700038D0, p + 0xD0, D_700038B0);
    if (func_0019AFE0(p, D_700038C0, D_700038D0, 7) & 6) {
        if (func_00178910(p, 1)) {
            *(unsigned char *)(p + 0x1F1) = 0;
            return 1;
        }
        goto ret0;
    }
    if (side == 0) {
        WD0 = 0.1f;
        WD1 = 0.0f;
        WD2 = 0.0f;
        WD3 = 0.0f;
    } else {
        WD0 = -0.1f;
        WD1 = 0.0f;
        WD2 = 0.0f;
        WD3 = 0.0f;
    }
    func_001026A0(D_70003900, p + 0xD0, D_70003910);
    func_001028B8(D_700038E0, D_700038C0, D_70003900);
    func_001028B8(D_700038F0, D_700038D0, D_70003900);
    if (func_0019AFE0(p, D_700038E0, D_700038F0, 7)) {
        return 1;
    }
    return 0xA;
    }
ret0:
    return 0;
}
