// NEARMISS func_0010FAD0  (vram 0x0010FAD0, 0x100 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.84% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc induction-variable + symbol-relocation idiom wall. 64/64 instrs match (99.84%); body fully decoded: retry loop over func_0010E6F8(s0,0x80000006,0)/func_0010E8A8(...9 args, 9th=0 on stack)/unaligned 4-byte memcpy D_00279440->D_00279668 (matched via __builtin_memcpy giving lwl/lwr/swl/swr). SINGLE residual: the ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_0010E6F8(void *p, unsigned int flags, int x);
extern int func_0010E8A8(void *p, int a1, int a2, int a3, int a4, void *a5, int a6, int a7, int a8);

extern int D_00241C98;
extern unsigned char D_00279640[];
extern unsigned char D_00279440[];
extern unsigned char D_00279668[];

int func_0010FAD0(void) {
    void *s0;
    int r;

    if (D_00241C98 < 0) {
        for (;;) {
            s0 = (void *)D_00279640;
            if (func_0010E6F8(s0, 0x80000006u, 0) < 0)
                return -1;
            r = *(int *)((char *)s0 + 0x24);
            if (r != 0) {
                D_00241C98 = 0;
                if (func_0010E8A8(s0, 0xFF, 0, 0, 0, (void *)D_00279440, 4, 0, 0) < 0)
                    return 0xFFFEFFFF;
                __builtin_memcpy((void *)D_00279668, (void *)D_00279440, 4);
                return 0;
            }
            {
                int i = 0x00100000;
                do {
                    i--;
                } while (i != -1);
            }
        }
    }
    return 0;
}
