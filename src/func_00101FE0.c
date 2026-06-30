// NEARMISS func_00101FE0  (vram 0x00101FE0, 0xE4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 64.70% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// Two stacked compiler-internal walls, both non-permutable. (1) Degenerate do{}while(0) inner masking loop: expected preserves a real CFG loop with a literal-zero guard ('move v0,zero; nop; nop; bnez v0; sw v1'); ee-gcc 2.9 -O2 eliminates clean structured while(0), and the closest self-clearing form 'do{ x &= ~0x100; ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern char D_FFFFFF[];
extern char D_0026B220[];
extern void func_00122B58(void *p);

void func_00101FE0(unsigned int *p, int a1, int a2) {
    int i = (int)&D_FFFFFF;

    while (p[0] & 0x100) {
        if (i < 0) {
            unsigned int x;
            func_00122B58(D_0026B220);
            x = p[0];
            if ((x >> 8) & 1) {
                do {
                    x = x & ~0x100;
                } while (x & 0x100);
                p[0] = x;
            }
        }
        i--;
    }

    if (p[4] != 0xFFFFFFFF) {
    } else {
        p[4] = a1;
    }
    p[8] = a2;
    p[0] = (p[0] & ~0xC) | 0x101;
}
