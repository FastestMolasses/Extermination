// NEARMISS func_00112F98  (vram 0x00112F98, 0x2E8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 70.56% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// TWO stacked walls. (1) eegcc LICM + callee-saved coloring wall: expected hoists NINE values into s0-s8 (frame 0xB0) — hi(D_0027B050), the full la(&D_0027AF88), la(&D_0027B050), hi(D_00241D3C), hi(D_00241D38), hi(D_0026BD00), hi(D_00241CF8), hi(D_0027AB40), mode; ours hoists only eight (frame 0xA0...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// libpad: padInit(mode) — binds the pad RPC server (sid 0x80000592) with a
// busy-wait retry loop, issues fno 0 with the init mode, then decodes the
// module-version block the IOP wrote back into the uncached receive buffer.

extern int D_00241CF8;
extern int D_00241D08;
extern int D_00241D0C;
extern int D_00241D10;
extern int D_00241D18;
extern int D_00241D1C;
extern int D_00241D20;
extern int D_00241D24;
extern int D_00241D28;
extern int D_00241D2C;
extern int D_00241D30;
extern int D_00241D34;
extern int D_00241D38;
extern int D_00241D3C;
extern int D_00279B00;
extern char D_0026BD00[];
extern char D_0026BD20[];
extern unsigned char D_0027AB40[];
extern unsigned char D_0027AF88[];
extern int D_0027B050;

extern int func_00112DC0(int arg);
extern void func_0010E088(int arg);
extern int ReleaseWaitThread(void);
extern int func_0010E6F8(void *bd, unsigned int sid, unsigned int mode);
extern void func_0010DFD8(void *addr, int size);
extern int func_0010E8A8(void *bd, int fno, int mode, void *send, int ssize,
                         void *recv, int rsize, void *end_func, void *end_para);
extern void func_00122B58(const char *fmt, ...);
extern void func_00111950(void);
extern void func_001118B8(void);
extern void func_00111AE0(void);

#define DELAY()                                                                \
    {                                                                          \
        int d = 0x100000;                                                      \
        do {                                                                   \
            d--;                                                               \
            __asm__ __volatile__("nop");                                       \
            __asm__ __volatile__("nop");                                       \
            __asm__ __volatile__("nop");                                       \
            __asm__ __volatile__("nop");                                       \
        } while (d != -1);                                                     \
    }

int func_00112F98(int mode)
{
    int r;
    int ret;
    int kind, va, vb;

    if (func_00112DC0(1) != 0) {
        return 0;
    }
    func_0010E088(0);
    D_00279B00 = ReleaseWaitThread();

    D_00241D1C = -1;
    D_00241D18 = 1;
    D_00241D20 = -1;
    D_00241D24 = -1;
    D_00241D28 = -1;
    D_00241D2C = -1;
    D_00241D30 = -1;
    D_00241D34 = -1;
    D_00241D3C = D_00241D3C + 1;
    D_00241D38 = -1;

    for (;;) {
        r = func_0010E6F8(D_0027AF88, 0x80000592, 0);
        if (r < 0) {
            if (D_00241CF8 > 0) {
                func_00122B58(D_0026BD00, r, D_00241D3C);
            }
            DELAY();
            continue;
        }
        if (*(int *)(D_0027AF88 + 0x24) != 0) {
            D_0027B050 = mode;
            D_00241D38 = 0;
            func_0010DFD8(&D_0027B050, 4);
            if (func_0010E8A8(D_0027AF88, 0, 0, &D_0027B050, 4, D_0027AB40,
                              0x10, 0, 0) < 0) {
                D_00241D18 = 0;
                return 0;
            }

            kind = *(int *)((unsigned int)(D_0027AB40 + 0xC) | 0x20000000);
            va = *(int *)((unsigned int)(D_0027AB40 + 0x4) | 0x20000000);
            vb = *(int *)((unsigned int)(D_0027AB40 + 0x8) | 0x20000000);
            ret = 1;
            if (kind != 0xFF) {
                if (kind == 0xFE) {
                    D_00241CF8 = ret;
                } else if (va / 256 < 2 || vb / 256 < 2) {
                    ret = 2;
                }
            }
            D_00241D18 = 0;

            switch (mode) {
            case 0:
            case 1:
                break;
            case 5:
                if (D_00241CF8 > 0) {
                    func_00122B58(D_0026BD20);
                }
                func_00111950();
                D_00241D08 = -1;
                D_00241D0C = -1;
                D_00241D10 = -1;
                return ret;
            }
            func_001118B8();
            func_00111AE0();
            return ret;
        }
        DELAY();
    }
}
