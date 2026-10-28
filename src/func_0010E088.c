// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96

//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 89.14% NEARMISS).
// Fixes read from the original instructions:
//  - each of the two interrupt-disable spin loops reads the status register
//    into its own local (one shared local changed register allocation);
//  - the descriptor is filled in field order +0x4..+0x24, with the +0x0 word
//    (= 1) written last;
//  - the original fills the call delay slot after `D_00241C80 = 1` with `ei`,
//    which only the 2.96 gas does (`// EEGCC_AS: 2.96`).
//
// Once-only SIF/RPC setup: guarded by D_00241C80 under disabled interrupts,
// calls func_0010D9A0, fills the D_00278BC0 descriptor (uncached pointers to
// three 0x20-entry tables), registers four handlers via func_0010DCA8, and,
// unless SIF register 0x80000002 is already set, posts a request, polls
// func_0010D958 until it completes and sets that register to 1.
//

typedef struct Ctx {
    /* 0x00 */ int   f0;
    /* 0x04 */ void *f4;
    /* 0x08 */ int   f8;
    /* 0x0C */ int   fC;
    /* 0x10 */ int   f10;
    /* 0x14 */ void *f14;
    /* 0x18 */ int   f18;
    /* 0x1C */ void *f1C;
    /* 0x20 */ int   f20;
    /* 0x24 */ int   f24;
    /* 0x28 */ void *f28;
} Ctx;

extern int  D_00241C80;
extern Ctx  D_00278BC0;
extern char D_002773C0[];
extern char D_00277BC0[];
extern char D_002783C0[];

extern void func_0010D9A0(void);
extern void func_0010DCA8(int, void (*)(), void *);
extern unsigned int sceSifGetReg(unsigned int);
extern void func_0010DE38(int, void *, int, int, int, int);
extern int  func_0010D958(int);
extern int  sceSifSetReg(int, int);
extern void func_0010E3A8();
extern void func_0010E648();
extern void func_0010E818();
extern void func_0010E460();

void func_0010E088(int mode)
{
    Ctx *ctx;
    char *p;
    unsigned int stat;
    unsigned int stat2;

    do {
        __asm__ __volatile__("di");
        __asm__ __volatile__("sync.p");
        __asm__ __volatile__("mfc0 %0, $12" : "=r"(stat));
    } while (stat & 0x10000);

    if (D_00241C80 != 0) {
        __asm__ __volatile__("ei");
        return;
    }
    D_00241C80 = 1;
    __asm__ __volatile__("ei");
    func_0010D9A0();

    do {
        __asm__ __volatile__("di");
        __asm__ __volatile__("sync.p");
        __asm__ __volatile__("mfc0 %0, $12" : "=r"(stat2));
    } while (stat2 & 0x10000);

    ctx = &D_00278BC0;
    ctx->f4  = (void *)((unsigned int)D_002773C0 | 0x20000000);
    ctx->f8  = 0x20;
    ctx->fC  = 0;
    ctx->f10 = 0;
    ctx->f14 = (void *)((unsigned int)D_00277BC0 | 0x20000000);
    ctx->f18 = 0x20;
    ctx->f1C = (void *)((unsigned int)D_002783C0 | 0x20000000);
    ctx->f20 = 0x20;
    ctx->f24 = 0;
    ctx->f0  = 1;

    func_0010DCA8(0x80000008, func_0010E3A8, ctx);
    func_0010DCA8(0x80000009, func_0010E648, ctx);
    func_0010DCA8(0x8000000A, func_0010E818, ctx);
    func_0010DCA8(0x8000000C, func_0010E460, ctx);

    __asm__ __volatile__("ei");

    if (sceSifGetReg(0x80000002) != 0) {
        return;
    }

    p = D_002773C0 + 0x40;
    *(int *)(p + 0xC) = 1;
    func_0010DE38(0x80000002, p, 0x10, 0, 0, 0);

    while (func_0010D958(0) == 0) {
        ;
    }
    sceSifSetReg(0x80000002, 1);
}
