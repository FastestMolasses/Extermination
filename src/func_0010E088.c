// NEARMISS func_0010E088  (vram 0x0010E088, ?) — readable decompilation, NOT byte-identical.
//
// objdiff 89.14% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// TWO stacked walls; body/structure fully recovered (prologue, both DI spin loops, the D_00241C80 once-only guard, all four func_0010DCA8 handler registrations, the RFU116 early-out, the func_0010D958 poll loop and the `j SetVSyncFlag` sibling call all byte-match). (1) Same eegcc ASSEMBLER delay-sl...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

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
extern int  RFU116(int);
extern void func_0010DE38(int, void *, int, int, int, int);
extern int  func_0010D958(int);
extern int  SetVSyncFlag(int, int);
extern void func_0010E3A8();
extern void func_0010E648();
extern void func_0010E818();
extern void func_0010E460();

void func_0010E088(void)
{
    Ctx *ctx;
    char *p;
    unsigned int stat;

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
        __asm__ __volatile__("mfc0 %0, $12" : "=r"(stat));
    } while (stat & 0x10000);

    ctx = &D_00278BC0;
    ctx->f0  = 1;
    ctx->f4  = (void *)((unsigned int)D_002773C0 | 0x20000000);
    ctx->f8  = 0x20;
    ctx->fC  = 0;
    ctx->f10 = 0;
    ctx->f14 = (void *)((unsigned int)D_00277BC0 | 0x20000000);
    ctx->f18 = 0x20;
    ctx->f1C = (void *)((unsigned int)D_002783C0 | 0x20000000);
    ctx->f20 = 0x20;
    ctx->f24 = 0;

    func_0010DCA8(0x80000008, func_0010E3A8, ctx);
    func_0010DCA8(0x80000009, func_0010E648, ctx);
    func_0010DCA8(0x8000000A, func_0010E818, ctx);
    func_0010DCA8(0x8000000C, func_0010E460, ctx);

    __asm__ __volatile__("ei");

    if (RFU116(0x80000002) != 0) {
        return;
    }

    p = D_002773C0 + 0x40;
    *(int *)(p + 0xC) = 1;
    func_0010DE38(0x80000002, p, 0x10, 0, 0, 0);

    while (func_0010D958(0) == 0) {
        ;
    }
    SetVSyncFlag(0x80000002, 1);
}
