// NEARMISS func_00107648  (vram 0x00107648, 0x3DC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 78.40% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc cross-jump-merge + list-scheduler wall (0x3C8 vs 0x3DC, 5 insns short). Body/structure/control flow fully recovered (verified branch-for-branch through the two top-level arms). Two hard residuals. (1) CROSS-JUMP MERGE: the target keeps two textually identical `ret = 1; if (X[0x28] != 1) ret...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char *D_002412F4;

extern int *D_00241300;
extern int *D_00241304;
extern int *D_00241308;
extern int *D_0024130C;
extern int *D_00241310;
extern int *D_00241314;
extern int *D_00241318;
extern int *D_0024131C;
extern int *D_00241320;

extern int D_00241448;
extern int D_00241470;
extern int D_00241474;
extern int D_0024147C;
extern int D_002414AC;
extern int D_002414B0;
extern int D_002414C0;
extern int D_002414C8;
extern int D_002414E8[];
extern int D_002414F8[];
extern int D_00241530;
extern int D_00241534;
extern unsigned char *D_00241544;
extern unsigned char *D_00241548;
extern unsigned char *D_0024154C;
extern long long D_00241B88;
extern long long D_00241B90;

int func_00107648(int flip)
{
    unsigned char *ctx;
    unsigned char *dst;
    int *tmp;
    int *sel;
    int need;
    int cur;
    int ret;
    int c2C, c30, c34, c38, c3C, c40, c5C, c60;
    long long q18, q20;

    need = (D_002414AC == 3) ? 2 : 4;
    ctx = *(unsigned char **)(D_002412F4 + 0x40);
    dst = 0;
    ret = 0;

    if (D_0024147C == 3) {
        D_00241544 = (unsigned char *)D_00241308;
        D_00241548 = (unsigned char *)D_00241314;
        D_0024154C = (unsigned char *)D_00241320;

        if (*(int *)(ctx + 0xA0) + *(int *)(ctx + 0xA4) >= need) {
            *(int *)(ctx + 0xE8) = 0;
            D_00241534 = 0;
            D_00241530 = 0;
        }

        if (*(int *)(ctx + 0xE8) != 0 || D_00241534 != 0) {
            if (D_00241530 == 0) {
                D_00241300[10] = 0;
                D_0024130C[10] = 0;
                D_00241318[10] = 0;
            }
        }
        *(int *)(ctx + 0xE8) = 0;
        cur = D_002414AC;
        D_00241534 = 0;

        if (cur == 3) {
            if (D_00241300[10] == 1 || D_00241530 != 0) {
                ret = 1;
                if (D_00241304[10] != 1)
                    ret = 0;
            }
        } else {
            if ((D_0024130C[10] == 1 && D_00241318[10] == 1) ||
                D_00241530 != 0) {
                if (D_00241310[10] == 1) {
                    ret = 1;
                    if (D_0024131C[10] != 1)
                        ret = 0;
                }
            }
        }
    } else {
        if (flip == 0) {
            tmp = D_00241300;
            D_00241300 = D_00241304;
            D_00241304 = tmp;
            tmp = D_0024130C;
            D_0024130C = D_00241310;
            D_00241310 = tmp;
            tmp = D_00241318;
            D_00241318 = D_0024131C;
            D_0024131C = tmp;
        }

        D_00241544 = (unsigned char *)D_00241304;
        D_00241548 = (unsigned char *)D_00241310;
        D_0024154C = (unsigned char *)D_0024131C;

        if (D_002414AC == 3) {
            if (D_0024147C != 2)
                ret = 1;
            else if (D_00241300[10] == 1)
                ret = 1;
        } else {
            sel = (D_002414AC == 1) ? D_0024131C : D_00241310;
            if (D_0024147C != 2)
                ret = 1;
            else if (flip != 0 && sel[10] == 1)
                ret = 1;
            else if (D_0024130C[10] == 1 && D_00241318[10] == 1)
                ret = 1;
        }
    }

    switch (D_002414AC) {
    case 1:
        dst = D_00241548;
        break;
    case 2:
        dst = D_0024154C;
        break;
    case 3:
        dst = D_00241544;
        break;
    }

    c2C = D_0024147C;
    c30 = D_002414AC;
    c34 = D_00241448;
    c38 = D_002414C8;
    c3C = D_002414B0;
    c40 = D_002414C0;
    q18 = D_00241B88;
    q20 = D_00241B90;
    c5C = D_00241470;
    c60 = D_00241474;

    *(int *)(dst + 0x2C) = c2C;
    *(int *)(dst + 0x30) = c30;
    *(int *)(dst + 0x34) = c34;
    *(int *)(dst + 0x38) = c38;
    *(int *)(dst + 0x3C) = c3C;
    *(int *)(dst + 0x40) = c40;
    *(int *)(dst + 0x28) = 0;
    *(int *)(dst + 0x44) = D_002414E8[0];
    *(int *)(dst + 0x48) = D_002414E8[1];
    *(long long *)(dst + 0x18) = q18;
    *(int *)(dst + 0x4C) = D_002414E8[2];
    *(long long *)(dst + 0x20) = q20;
    *(int *)(dst + 0x50) = D_002414F8[0];
    *(int *)(dst + 0x54) = D_002414F8[1];
    *(int *)(dst + 0x60) = c60;
    *(int *)(dst + 0x58) = D_002414F8[2];
    *(int *)(dst + 0x5C) = c5C;

    return ret;
}
