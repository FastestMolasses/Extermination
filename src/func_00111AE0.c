// NEARMISS func_00111AE0  (vram 0x00111AE0, 0x148 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.56% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// splat D_FFFFF pseudo-symbol wall (HARD, unreachable from C) + GPR coloring. libcdvd-family init: sceSifInitRpc(0); if D_00241D1C < 0, retry-bind loop on RPC dest 0x80000596 (cdvdfsv) — printf(D_0026BB30) when the debug level D_00241CF8 > 0 and a hand-rolled nop busy-wait on bind failure, plus up ...
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
extern void func_001115D0(void *);

extern int D_00241D1C;
extern int D_00241CF8;
extern unsigned char D_00279BE0[];
extern int D_00279C08;
extern char D_0026BB30[];

int func_00111AE0(void)
{
    unsigned char *cd;
    int retry;
    int i;

    func_0010E088(0);

    if (D_00241D1C < 0) {
        retry = 0;
        for (;;) {
            cd = D_00279BE0;
            if (func_0010E6F8(cd, 0x80000596u, 0) < 0) {
                if (D_00241CF8 > 0)
                    func_00122B58(D_0026BB30);
                i = 0x100000;
                do {
                    i--;
                    __asm__("nop");
                    __asm__("nop");
                    __asm__("nop");
                    __asm__("nop");
                } while (i != -1);
                continue;
            }
            if (*(int *)(cd + 0x24) != 0)
                break;
            i = 0x100000;
            do {
                i--;
                __asm__("nop");
                __asm__("nop");
                __asm__("nop");
                __asm__("nop");
            } while (i != -1);
            if (++retry > 0x11)
                return 0;
        }
        D_00241D1C = 0;
    }

    D_00279C08 = 0xB;
    if (func_0010E8A8(D_00279BE0, 1, 1, 0, 0, 0, 0, func_001115D0,
                      &D_00279C08) < 0)
        return 0;
    return 1;
}
