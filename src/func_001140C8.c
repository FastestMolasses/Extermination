// NEARMISS func_001140C8  (vram 0x001140C8, 0x158 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.93% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// splat D_FFFFF pseudo-symbol wall (HARD, unreachable from C) + GPR live-range-split coloring. This is sceMcInit: sceSifInitRpc(0); retry-bind loop on RPC dest 0x80000400 (mcserv) with a hand-rolled nop busy-wait between tries and a printf+hang on bind failure; then sceSifCallRpc(cd, 0xFE, 0, &D_00...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_0010E088(int);
extern int func_0010E6F8(void *, unsigned int, int);
extern int func_0010E8A8(void *, int, int, void *, int, void *, int, void *, void *);
extern int func_00122B58(const char *, ...);

extern unsigned char D_0027B0C0[];
extern unsigned char D_0027B140[];
extern int D_0027C680[];
extern char D_0026BED8[];
extern char D_0026BEF0[];
extern char D_0026BF18[];

int func_001140C8(void)
{
    unsigned char *cd;
    int ret;
    int i;

    func_0010E088(0);

    for (;;) {
        if (func_0010E6F8(D_0027B0C0, 0x80000400u, 0) < 0) {
            func_00122B58(D_0026BED8);
            for (;;) {
                __asm__("nop");
                __asm__("nop");
                __asm__("nop");
                __asm__("nop");
                __asm__("nop");
            }
        }
        cd = D_0027B0C0;
        if (*(int *)(cd + 0x24) != 0)
            break;
        i = 0x100000;
        do {
            i--;
            __asm__("nop");
            __asm__("nop");
            __asm__("nop");
            __asm__("nop");
        } while (i != 0);
    }

    cd = D_0027B0C0;
    ret = func_0010E8A8(cd, 0xFE, 0, D_0027B140, 0x30, D_0027C680, 0xC, 0, 0);
    if (ret < 0) {
        *(int *)(cd + 0x24) = 0;
        return ret - 0x64;
    }

    if (D_0027C680[1] < 0x20A) {
        func_00122B58(D_0026BEF0);
        *(int *)(cd + 0x24) = 0;
        return -0x78;
    }

    if (D_0027C680[2] < 0x20E) {
        func_00122B58(D_0026BF18);
        *(int *)(cd + 0x24) = 0;
        return -0x79;
    }

    return D_0027C680[0];
}
