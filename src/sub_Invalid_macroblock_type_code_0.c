// NEARMISS sub_Invalid_macroblock_type_code_0  (vram 0xnvalid_macroblock_type_code_0, 0x524 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.39% via ee-gcc 2.9-991111-01 (-O2, 2.96 gas). Matching wave 2026-10-27 raised it from
// 94.02%: the callee-saved swap of pmode/arg5 was our bug (the C cached *pmode in a local; the
// original re-reads *pmode for each mode test), *pa is assigned from one ternary (not a
// pre-zeroed local), the slot table is a struct array (2 x 0x140-byte slots + active index),
// and dst[1] = 0 is hoisted ahead of the D_002414AC test in the tail.
// Remaining differences: in the final field-clear block the scheduler places the constant 3 for
// the D_002414AC compare one slot earlier than the original, which rotates which of the four
// zero stores lands in the branch delay slot (3 instructions; all 24 store orders, chained
// assignments, a combined guard and a pre-read local were tried).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// EEGCC_AS: 2.96
// (2.96 gas: matching wave 2026-10-27 sweep; it fills call/return delay slots as the original does)
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
struct mb_buf {
    int w0;
    int addr;
    int pad[77];
    int done;
};
struct mb_bufs {
    struct mb_buf e[2];
    int cur;
};
extern struct mb_bufs D_002418F8;
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
    int t3;
    int t4;
    int t7;
    int tf;
    int v;
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

    if (D_002414AC == 3) {
        t3 = (*pmode == 1) ? 2 : 1;
        t4 = (*pmode == 2);
    } else {
        t4 = 0;
        t3 = (*pmode == 2) ? 2 : 1;
    }
    tf = (*pmode == 3);
    t7 = 0;
    if (!t4)
        t7 = (D_002414AC == 3);

    *pa = (D_002414AC == 3 && D_002414B4 == 0 && (*flags & 3) != 0)
          ? func_00106948(1) : 0;

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
        *(volatile int *)0x1000B010 = (D_002418F8.e[D_002418F8.cur].addr & 0x0FFFFFFF) | 0x80000000;
        *(volatile int *)0x1000B020 = 0x30;
        *(volatile int *)0x1000B000 = 0x100;
        func_001063E8();
        func_001063B8(((*flags & 1) << 27) | (*pa << 25)
                      | ((D_0024153C << 26) | 0x20000000) | (D_00241540 << 16));
    } else {
        D_002418F8.e[D_002418F8.cur].done = 1;
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
    dst[1] = 0;
    dst[5] = 0;
    if (D_002414AC == 3) {
        *pmode = 2;
    } else {
        *pmode = 1;
        *pb = (D_002414AC == 2);
    }
    return 1;
}
