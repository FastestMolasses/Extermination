// NEARMISS func_001E7D20  (vram 0x001E7D20, 0xE18 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.08% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// GPR allocation permutation + delay-slot constant sharing. Body/structure fully recovered (all state-machine arms, the 32x32 ripple relaxation, the random-splash impulse and the whole GS packet builder verify against the asm). Sole residuals: (a) the target keeps the entity pointer in $9 while mwc...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Water-surface actor state machine (records at D_00275C20 + idx*0xA060,
// 32x32 height-field ripple sim + GS display-list builder).
//
// State e[4]: 0 = build the grid (positions, UVs, per-level tint/extent
// constants chosen from D_00810700/D_00810701), 1 = per-frame ripple step
// (damped wave relaxation over the 32x32 velocity/height fields, a random
// impulse splash, then normal/UV rebuild and the GS packet emission),
// 2/3 = idle.
//
// func_001E7C60 (the "flood the whole grid to a constant height" helper) is
// defined `static` here on purpose: it lives in the same translation unit in
// the original, and mwcc's intra-TU register-usage analysis is what lets the
// caller keep the entity pointer in a caller-saved temp across the call.

typedef unsigned __int128 uint128;

extern int func_001E7CB0();
extern void func_001E8B90(float *vec, float speed);
extern int func_00122BB8(void);
extern char *func_001CB5F0(char *a0, int a1, int a2);
extern void func_00102948(void *dst, void *src);
extern void copy_qw4(void *dst, void *src);
extern void func_001CB950(char *a0, int a1, long a2);
extern void func_001CB6B0(char *a0, int a1, int a2, int a3);
extern void func_001CB760(char *a0, int a1, char *a2);
extern int func_001D2E00(int n);
extern void func_001D2DE0(int n, int v);

extern char *D_00275C20;
extern char *D_00275670;
extern char *D_00275674;
extern int D_00275C10;
extern int D_00275C14;
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];
extern unsigned char D_008107F5[];
extern unsigned char D_008107F6[];
extern char D_007635C0[];
extern char D_008105D0[];
extern char D_002553B0[];
extern char D_00234B00[];
extern char D_70003AC0[];

static void func_001E7C60(char *p, float v) {
    int j;
    int i;
    char *col;
    char *row;
    char *b2;

    i = 0;
    row = p;
    b2 = p + 8;
    do {
        j = 0;
        col = row;
        do {
            j++;
            *(float *)(col + 0x64) = v;
            col += 0x10;
            *(float *)(p + 4) = v;
            *(float *)b2 = v;
        } while (j < 0x20);
        i++;
        row += 0x200;
    } while (i < 0x20);
}

void func_001E7D20(char *arg0) {
    char *s;
    float vec[4];
    long tab[15];
    uint128 *qd;
    uint128 *qs;
    int i;
    int j;
    char *c1;
    char *c2;
    char *r1;
    char *r2;
    char *rm;
    char *rp;
    int im1;
    int ip1;
    int jm1;
    int jp1;
    float x;
    float z;
    float f;
    float k4;
    float damp;
    float coef;
    float sv;
    float su;
    int rrow;
    int rcol;
    float k;
    char *vc;
    char *vm;
    char *vp;
    char *blk;
    char *dst;
    char *row1;
    char *row2;
    char *r0;
    char *row0;
    int strip;
    int key;
    int n;

    s = D_00275C20 + *(unsigned char *)(arg0 + 0xD) * 0xA060;
    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        *(char **)(s + 0x58) = arg0;
        *(float *)(s + 0x0) = *(float *)(arg0 + 0xB0);
        *(float *)(s + 0x4) = *(float *)(arg0 + 0xB4);
        *(float *)(s + 0x8) = *(float *)(arg0 + 0xB8);
        *(float *)(s + 0x30) = *(float *)(arg0 + 0xC0);
        *(float *)(s + 0x34) = *(float *)(arg0 + 0xC8);
        *(float *)(s + 0x20) = 0.961f;
        *(float *)(s + 0x24) = 0.022f;
        *(float *)(s + 0x2C) = 0.4f;
        *(float *)(s + 0x28) = 0.25f;
        *(unsigned char *)(arg0 + 5) = 0;
        *(int *)(arg0 + 0x20) = 0;
        switch (D_00810700[0]) {
        case 0:
            if (D_00810701[0] != 1) {
                *(float *)(s + 0x10) = 32.0f;
                *(float *)(s + 0x14) = 48.0f;
                *(float *)(s + 0x18) = 48.0f;
                *(float *)(s + 0x1C) = 80.0f;
                *(float *)(s + 0x3C) = 0.265f;
                *(float *)(s + 0x38) = 0.265f;
            } else {
                *(float *)(s + 0x10) = 148.0f;
                *(float *)(s + 0x14) = 91.0f;
                *(float *)(s + 0x18) = 70.0f;
                *(float *)(s + 0x1C) = 89.0f;
                *(float *)(s + 0x3C) = 0.21f;
                *(float *)(s + 0x38) = 0.25f;
            }
            break;
        default:
        case 1:
            *(float *)(s + 0x10) = 36.0f;
            *(float *)(s + 0x14) = 34.0f;
            *(float *)(s + 0x18) = 62.0f;
            *(float *)(s + 0x1C) = 88.0f;
            *(float *)(s + 0x3C) = 0.325f;
            *(float *)(s + 0x38) = 0.325f;
            break;
        case 6:
            *(float *)(s + 0x10) = 36.0f;
            *(float *)(s + 0x14) = 34.0f;
            *(float *)(s + 0x18) = 62.0f;
            *(float *)(s + 0x1C) = 40.0f;
            *(float *)(s + 0x3C) = 0.325f;
            *(float *)(s + 0x38) = 0.325f;
            break;
        case 0x13:
            *(float *)(s + 0x1C) = 128.0f;
            *(float *)(s + 0x18) = 128.0f;
            *(float *)(s + 0x14) = 128.0f;
            *(float *)(s + 0x10) = 128.0f;
            *(float *)(s + 0x3C) = 0.325f;
            *(float *)(s + 0x38) = 0.325f;
            *(float *)(s + 0x2C) = 2.5f;
            switch (*(unsigned char *)(arg0 + 0xD)) {
            case 0:
                if (D_008107F6[0] == 0xFF) {
                    *(unsigned char *)(arg0 + 5) = 2;
                    *(float *)(arg0 + 0xB4) = 132.0f;
                }
                break;
            case 1:
                if (D_008107F5[0] == 0xFF) {
                    *(unsigned char *)(arg0 + 5) = 2;
                    *(float *)(arg0 + 0xB4) = 160.0f;
                }
                break;
            }
            func_001E7C60(s, *(float *)(arg0 + 0xB4));
            break;
        }
        i = 0;
        r1 = s;
        r2 = s;
        do {
            j = 0;
            c1 = r1;
            c2 = r2;
            do {
                switch (D_00810700[0]) {
                case 0x13:
                    x = *(float *)(s + 0x0) + *(float *)(s + 0x30) * (float)j / 30.0f;
                    z = *(float *)(s + 0x8) + *(float *)(s + 0x34) * (float)i / 30.0f;
                    if (*(unsigned char *)(arg0 + 0xD) == 1 && z <= 921.0f && x >= 930.0f) {
                        x = 930.0f;
                    }
                    break;
                default:
                    x = *(float *)(s + 0x0) + *(float *)(s + 0x30) * (float)j / 30.0f;
                    z = *(float *)(s + 0x8) + *(float *)(s + 0x34) * (float)i / 30.0f;
                    break;
                }
                *(float *)(c1 + 0x60) = x;
                *(float *)(c1 + 0x68) = z;
                *(float *)(c1 + 0x64) = *(float *)(s + 0x4);
                *(float *)(c1 + 0x6C) = 1.0f;
                *(float *)(c1 + 0x4060) = (float)j / 32.0f;
                *(float *)(c1 + 0x4064) = (float)i / 32.0f;
                *(float *)(c2 + 0x8060) = 0.0f;
                *(float *)(c2 + 0x9060) = 0.0f;
                j += 1;
                c1 += 0x10;
                c2 += 4;
            } while (j < 0x20);
            i += 1;
            r1 += 0x200;
            r2 += 0x80;
        } while (i < 0x20);
        *(char *)(arg0 + 0xC) = 0;
        *(char *)(arg0 + 9) = 0;
        *(unsigned char *)(arg0 + 4) = 1;
        break;
    case 1:
        *(int *)(s + 0x54) = 1;
        if (D_00810700[0] == 0x13) {
            switch (*(unsigned char *)(arg0 + 0xD)) {
            case 0:
                switch (*(unsigned char *)(arg0 + 5)) {
                case 0:
                case 2:
                    break;
                case 1:
                    f = *(float *)(arg0 + 0xB4);
                    if (f <= 132.0f || D_008107F6[0] == 0xFF) {
                        *(float *)(arg0 + 0xB4) = 132.0f;
                        *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                    } else {
                        *(float *)(arg0 + 0xB4) = f - 0.1f;
                    }
                    func_001E7C60(s, *(float *)(arg0 + 0xB4));
                    break;
                }
                break;
            case 1:
                switch (*(unsigned char *)(arg0 + 5)) {
                case 0:
                case 2:
                    break;
                case 1:
                    f = *(float *)(arg0 + 0xB4);
                    if (f <= 160.0f || D_008107F5[0] == 0xFF) {
                        *(float *)(arg0 + 0xB4) = 160.0f;
                        *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                    } else {
                        *(float *)(arg0 + 0xB4) = f - 1.0f;
                    }
                    func_001E7C60(s, *(float *)(arg0 + 0xB4));
                    break;
                }
                vec[0] = 877.5f + 62.200012f * (4.656613e-10f * (float)func_00122BB8());
                vec[1] = 0.0f;
                vec[2] = 982.0f + 37.400024f * (4.656613e-10f * (float)func_00122BB8());
                vec[3] = 1.0f;
                func_001E8B90(vec, 0.1f + 1.1f * (4.656613e-10f * (float)func_00122BB8()));
                break;
            }
        }
        k4 = 4.0f + *(float *)(s + 0x28);
        damp = *(float *)(s + 0x20);
        coef = *(float *)(s + 0x24);
        sv = *(float *)(s + 0x38);
        su = *(float *)(s + 0x3C);
        i = 0;
        r1 = s;
        r2 = s;
        do {
            im1 = i - 1;
            ip1 = i + 1;
            if (im1 < 0) {
                im1 = 0;
            }
            if (ip1 >= 0x20) {
                ip1 = 0x1F;
            }
            rm = s + (im1 << 7);
            rp = s + (ip1 << 7);
            j = 0;
            c1 = r1;
            c2 = r2;
            do {
                jm1 = j - 1;
                jp1 = j + 1;
                if (jm1 < 0) {
                    jm1 = 0;
                }
                if (jp1 >= 0x20) {
                    jp1 = 0x1F;
                }
                j += 1;
                *(float *)(c1 + 0x9060) = *(float *)(c1 + 0x9060) * damp;
                *(float *)(c1 + 0x9060) = *(float *)(c1 + 0x9060) +
                    (*(float *)(rp + 0x8060) + (*(float *)(rm + 0x8060) +
                     (*(float *)(r1 + (jm1 << 2) + 0x8060) + *(float *)(r1 + (jp1 << 2) + 0x8060))) -
                     *(float *)(c1 + 0x8060) * k4) * coef;
                *(float *)(c2 + 0x4060) = su +
                    0.085f * (*(float *)(r1 + (jm1 << 2) + 0x8060) - *(float *)(r1 + (jp1 << 2) + 0x8060));
                *(float *)(c2 + 0x4064) = sv +
                    0.085f * (*(float *)(rm + 0x8060) - *(float *)(rp + 0x8060));
                c1 += 4;
                rm += 4;
                rp += 4;
                c2 += 0x10;
            } while (j < 0x20);
            i += 1;
            r1 += 0x80;
            r2 += 0x200;
        } while (i < 0x20);
        rrow = (func_00122BB8() >> 16 << 5) >> 15;
        rcol = (func_00122BB8() >> 16 << 5) >> 15;
        jm1 = rcol - 1;
        jp1 = rcol + 1;
        im1 = rrow - 1;
        ip1 = rrow + 1;
        if (jm1 < 0) {
            jm1 = 0;
        }
        if (jp1 >= 0x20) {
            jp1 = 0x1F;
        }
        if (im1 < 0) {
            im1 = 0;
        }
        if (ip1 >= 0x20) {
            ip1 = 0x1F;
        }
        k = *(float *)(s + 0x2C);
        vc = (rrow << 7) + s + 0x9060;
        vm = (im1 << 7) + s + 0x9060;
        vp = (ip1 << 7) + s + 0x9060;
        *(float *)(vc + (rcol << 2)) += k;
        k = k * 0.5f;
        *(float *)(vm + (rcol << 2)) += k;
        *(float *)(vp + (rcol << 2)) += k;
        *(float *)(vc + (jm1 << 2)) += k;
        *(float *)(vc + (jp1 << 2)) += k;
        k = k * 0.5f;
        *(float *)(vm + (jm1 << 2)) += k;
        *(float *)(vp + (jm1 << 2)) += k;
        *(float *)(vm + (jp1 << 2)) += k;
        *(float *)(vp + (jp1 << 2)) += k;
        i = 0;
        r1 = s;
        r2 = s;
        do {
            j = 0;
            c1 = r1;
            c2 = r2;
            do {
                j += 1;
                *(float *)(c1 + 0x8060) = *(float *)(c1 + 0x8060) + *(float *)(c1 + 0x9060);
                *(float *)(c2 + 0x64) = *(float *)(s + 0x4) + 0.02f * *(float *)(c1 + 0x8060);
                c1 += 4;
                c2 += 0x10;
            } while (j < 0x20);
            i += 1;
            r1 += 0x80;
            r2 += 0x200;
        } while (i < 0x20);
        if (func_001E7CB0(s) != 0) {
            strip = 0;
            row0 = s;
            do {
                blk = func_001CB5F0(D_007635C0, 0x1000, 0x62);
                *(int *)(blk + 0x0) = 0;
                *(int *)(blk + 0x4) = 0;
                *(int *)(blk + 0x8) = 0x01000404;
                *(int *)(blk + 0xC) = 0x6C608000;
                row1 = s + ((strip + 1) << 9);
                dst = blk + 0x10;
                row2 = s + ((strip + 2) << 9);
                i = 0;
                r0 = row0;
                do {
                    func_00102948(dst, r0 + 0x60);
                    func_00102948(blk + ((i + 0x20) << 4) + 0x10, row1 + 0x60);
                    func_00102948(blk + ((i + 0x40) << 4) + 0x10, row2 + 0x60);
                    i += 1;
                    r0 += 0x10;
                    dst += 0x10;
                    row1 += 0x10;
                    row2 += 0x10;
                } while (i < 0x20);
                if (strip == 0x1D) {
                    *(int *)(blk + 0x610) = 0x14000000;
                } else {
                    *(int *)(blk + 0x610) = 0x17000000;
                }
                strip += 1;
                *(int *)(blk + 0x614) = 0;
                *(int *)(blk + 0x618) = 0;
                row0 += 0x200;
                *(int *)(blk + 0x61C) = 0;
            } while (strip < 0x1E);
            blk = func_001CB5F0(D_007635C0, 0x1000, 5);
            *(int *)(blk + 0x0) = 0;
            *(int *)(blk + 0x4) = 0;
            *(int *)(blk + 0x8) = 0x01000404;
            *(int *)(blk + 0xC) = 0x6C040000;
            copy_qw4(blk + 0x10, D_70003AC0);
            blk = func_001CB5F0(D_007635C0, 0x1000, 9);
            *(int *)(blk + 0x0) = 0;
            *(int *)(blk + 0x4) = 0;
            *(int *)(blk + 0x8) = 0x01000404;
            *(int *)(blk + 0xC) = 0x6C0803F8;
            switch (D_00810700[0]) {
            case 6:
                *(float *)(blk + 0x10) = 24.0f;
                *(float *)(blk + 0x14) = 26.0f;
                *(float *)(blk + 0x18) = -1.6f;
                *(int *)(blk + 0x1C) = 0;
                break;
            case 0x13:
                *(float *)(blk + 0x10) = 34.8f;
                *(float *)(blk + 0x14) = 24.0f;
                *(float *)(blk + 0x18) = -1.5f;
                *(int *)(blk + 0x1C) = 0;
                break;
            default:
                *(float *)(blk + 0x10) = 48.0f;
                *(float *)(blk + 0x14) = 24.0f;
                *(float *)(blk + 0x18) = -1.5f;
                *(int *)(blk + 0x1C) = 0;
                break;
            }
            key = (D_00810700[0] << 8) + D_00810701[0];
            switch (key) {
            case 0:
                *(float *)(blk + 0x20) = 47.0f;
                *(float *)(blk + 0x24) = 44.0f;
                *(float *)(blk + 0x28) = 44.0f;
                break;
            case 1:
                *(float *)(blk + 0x20) = 127.0f;
                *(float *)(blk + 0x24) = 102.0f;
                *(float *)(blk + 0x28) = 81.0f;
                break;
            default:
                *(float *)(blk + 0x20) = 119.0f;
                *(float *)(blk + 0x24) = 119.0f;
                *(float *)(blk + 0x28) = 119.0f;
                break;
            }
            *(int *)(blk + 0x2C) = 0;
            *(float *)(blk + 0x30) = 0.44f;
            *(float *)(blk + 0x34) = 0.44f;
            *(float *)(blk + 0x38) = 0.35f;
            *(float *)(blk + 0x3C) = 0.245f;
            func_00102948(blk + 0x40, D_008105D0);
            *(long *)(blk + 0x50) = 0x8040 | (0x303E4000L << 32);
            *(long *)(blk + 0x58) = 0x412;
            func_00102948(blk + 0x60, D_00275670 + 0xA0);
            func_00102948(blk + 0x70, D_00275670 + 0x2220);
            func_00102948(blk + 0x80, D_00275670 + 0x2230);
            switch (D_00810700[0]) {
            case 0x13:
                qs = (uint128 *)D_002553B0;
                qd = (uint128 *)tab;
                n = 7;
                do {
                    n -= 1;
                    *qd = *qs;
                    qs += 1;
                    qd += 1;
                } while (n > 0);
                *(long *)qd = *(long *)qs;
                if (D_00275C14 != *(int *)0x70003B68) {
                    n = D_00275C10;
                    D_00275C14 = *(int *)0x70003B68;
                    if (n > 0) {
                    } else {
                        n = 0;
                    }
                    if (n >= 0xE) {
                        n = 0xE;
                    }
                    D_00275C10 = n;
                }
                func_001CB950(D_007635C0, 0x1000, tab[D_00275C10] & 0xFFFFFFFBFFFFFFFFL);
                break;
            default:
                func_001CB950(D_007635C0, 0x1000, (long)(0x9942 << 16 | 0x2040) | ((long)0x20048BA1 << 32));
                break;
            }
            if (func_001D2E00(2) != 0) {
                func_001CB6B0(D_007635C0, 0x1000, 8, (int)(D_00275674 + 0x8A0));
                func_001D2DE0(2, 0);
            } else {
                func_001CB6B0(D_007635C0, 0x1000, 8, (int)(D_00275674 + 0x720));
            }
            func_001CB760(D_007635C0, 0x1000, D_00234B00);
        }
        break;
    case 2:
    case 3:
        break;
    }
}
