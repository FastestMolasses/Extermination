// NEARMISS func_00101F08  (vram 0x00101F08, 0xD8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 66.39% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc degenerate dead-backedge codegen wall (NOT the branch-likely wall). The terminal bnel (branch-likely) IS reproduced and matches; the D_FFFFFF counter is correctly a relocated symbol address (lui/addiu %hi/%lo) not a literal; the >>8 is correctly srl (unsigned). Residual gaps: (1) expected keeps a degenerate in...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_0026B220;
extern char D_FFFFFF;
extern void func_00122B58(int);

void func_00101F08(int *s0, int arg1) {
    int i = (int)&D_FFFFFF;
    if (s0[0] & 0x100) {
        do {
            if (i < 0) {
                unsigned int v;
                func_00122B58((int)&D_0026B220);
                v = (unsigned int)s0[0];
                if ((v >> 8) & 1) {
                    s0[0] = s0[0] & ~0x100;
                }
            }
            i = i - 1;
        } while (s0[0] & 0x100);
    }
    if (*(int *)((char *)s0 + 0x30) != -1) {
        *(int *)((char *)s0 + 0x30) = arg1;
    }
    *(int *)((char *)s0 + 0x20) = 0;
    s0[0] = (s0[0] & ~0xC) | 0x105;
}
