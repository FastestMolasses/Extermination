// NEARMISS sub_PsIIlibpad_2000  (vram 0xsIIlibpad_2000, 0x134 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.74% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// splat D_FFFF pseudo-symbol wall (HARD, unreachable from C — same class as the documented D_FFFFF wall in docs/PROGRESS.md #2 / docs/NEARMISS.md). 73 of 77 instructions are identical, .text size identical (308 == 308), control flow / registers / delay slots all exact. The only 4 residual rows are ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_0010E6F8(int a0, int a1, int a2);
extern int func_001114B8(void);
extern void func_00122B58(char *fmt, ...);
extern int func_001107C8(int a0);

extern int D_00241CB0;
extern int D_00279700[];
extern char D_0026BA28[];
extern char D_0026BA50[];

int sub_PsIIlibpad_2000(int a0)
{
    int i;
    int r;

    D_00241CB0 = 1;

    for (;;) {
        func_0010E6F8((int)&D_00279700[0], 0x80000100, 0);
        if (D_00279700[9] != 0) {
            break;
        }
        i = 0x10000;
        do {
            i--;
            asm("nop");
            asm("nop");
            asm("nop");
            asm("nop");
        } while (i != -1);
    }

    for (;;) {
        func_0010E6F8((int)&D_00279700[10], 0x80000101, 0);
        if (D_00279700[19] != 0) {
            break;
        }
        i = 0x10000;
        do {
            i--;
            asm("nop");
            asm("nop");
            asm("nop");
            asm("nop");
        } while (i != -1);
    }

    r = func_001114B8();
    if ((r >> 8) != 3) {
        func_00122B58(D_0026BA28);
        func_00122B58(D_0026BA50, 3, 0, r >> 8, r & 0xFF);
        return 0;
    }
    return func_001107C8(a0);
}
