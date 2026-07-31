// NEARMISS sub_Invalid_macroblock_type_code_0  (vram 0xnvalid_macroblock_type_code_0, 0x524 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.09% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc GPR-coloring wall + one spill-reload/delay-slot divergence. The instruction sequence is otherwise identical end-to-end (both hardware-register RMWs, all four callee dispatches, the GIF/DMA channel setup, the field-clear blocks and the whole return tail). Residual is (a) a pure callee-saved ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241408;
extern int D_0024147C;
extern int D_00241484;
extern int D_00241488;
extern int D_0024148C;
extern int D_00241490;
extern int D_00241498[4];
extern int D_002414AC;
extern int D_002414B4;
extern int D_002414B8;
extern int D_0024153C;
extern int D_00241540;
extern int D_00241BB0;
extern char D_002418F8[];
extern char D_0026B490[];

extern int func_00106540(int nbits);
extern void func_0010A3A8(char *msg);
extern int func_00106948(int nbits);
extern void func_00106830(int nbits);
extern int func_001063E8(void);
extern void func_001063B8(unsigned int cmd);
extern void func_001060F8(int *dst, int a1, int *a2, int a3, int a4, int a5,
                          int a6, int a7, int a8, int a9);
extern void func_00106278(int *dst, int a1, int a2, int a3, int a4, int a5, int a6);

int sub_Invalid_macroblock_type_code_0(int *flags, int *pmode, int *pa, int *dst,
                                       int *pb, int arg5)
{
    int code;
    int mode;
    int t3;
    int t4;
    int t7;
    int tf;
    int v;
    char *p;
    int k;
    int *p540;

    *(int *)0x10002010 = (*(int *)0x10002010 & 0xF8FFFFFF) | (D_0024147C << 24);

    code = func_00106540(1);
    *flags = code;
    if (code == 0) {
        func_0010A3A8(D_0026B490);
        D_00241408 = 1;
        return 0;
    }

    if (code & 0xC) {
        if (D_002414AC == 3 && D_002414B4 != 0)
            *pmode = 2;
        else
            *pmode = func_00106948(2);
    } else if ((code & 1) && D_002414B8 != 0) {
        *pmode = (D_002414AC == 3) ? 2 : 1;
    }

    mode = *pmode;
    if (D_002414AC == 3) {
        t3 = (mode == 1) ? 2 : 1;
        t4 = (mode == 2);
    } else {
        t3 = (mode == 2) ? 2 : 1;
        t4 = 0;
    }
    tf = (mode == 3);
    t7 = 0;
    if (!t4)
        t7 = (D_002414AC == 3);

    v = 0;
    if (D_002414AC == 3 && D_002414B4 == 0 && (*flags & 3) != 0)
        v = func_00106948(1);
    *pa = v;

    if (*flags & 0x10) {
        p540 = &D_00241540;
        *p540 = func_00106948(5);
    }

    if ((*flags & 8) || ((*flags & 1) && D_002414B8 != 0)) {
        if (D_00241BB0 != 0)
            func_001060F8(dst, arg5, pb, 0, t3, t4,
                          D_00241498[0] - 1, D_00241498[1] - 1, tf, t7);
        else
            func_00106278(dst, arg5, D_00241488 - 1, D_00241488 - 1, 0, 0, D_00241484);
    }

    if (D_00241408 != 0)
        return 0;

    if (*flags & 4) {
        if (D_00241BB0 != 0)
            func_001060F8(dst, arg5, pb, 1, t3, t4,
                          D_00241498[2] - 1, D_00241498[3] - 1, 0, t7);
        else
            func_00106278(dst + 2, arg5, D_00241490 - 1, D_00241490 - 1, 0, 0, D_0024148C);
    }

    if (D_00241408 != 0)
        return 0;

    if ((*flags & 1) && D_002414B8 != 0)
        func_00106830(1);

    if (*flags & 3) {
        p = D_002418F8;
        k = *(int *)(p + 0x280) * 0x140;
        *(volatile int *)0x1000B010 = (*(int *)(p + k + 4) & 0x0FFFFFFF) | 0x80000000;
        *(volatile int *)0x1000B020 = 0x30;
        *(volatile int *)0x1000B000 = 0x100;
        func_001063E8();
        func_001063B8(((*flags & 1) << 27) | (*pa << 25)
                      | ((D_0024153C << 26) | 0x20000000) | (D_00241540 << 16));
    } else {
        p = D_002418F8;
        *(int *)(p + *(int *)(p + 0x280) * 0x140 + 0x13C) = 1;
    }

    D_0024153C = 0;
    if (D_00241408 != 0)
        return 0;

    if ((*flags & 1) == 0)
        D_0024153C = 1;

    if ((*flags & 1) && D_002414B8 == 0) {
        dst[5] = 0;
        dst[4] = 0;
        dst[1] = 0;
        dst[0] = 0;
        dst[7] = 0;
        dst[6] = 0;
        dst[3] = 0;
        dst[2] = 0;
    }

    if (D_0024147C != 2)
        return 1;
    if (*flags & 9)
        return 1;

    dst[0] = 0;
    dst[4] = 0;
    dst[5] = 0;
    if (D_002414AC == 3) {
        dst[1] = 0;
        *pmode = 2;
    } else {
        *pmode = 1;
        dst[1] = 0;
        *pb = (D_002414AC == 2);
    }
    return 1;
}
