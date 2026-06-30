// NEARMISS func_001118B8  (vram 0x001118B8, 0x94 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 77.30% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// gcc-2.9 branch-shape + delay-slot-fill wall. Frame matches (0x50) and call/store data flow is semantically correct. Two unmatched gcc codegen decisions: (1) inner test of D_00241D0C compiles to 'beq + b' to a private epilogue instead of expected 'bne' into the shared epilogue (which did 'ld ra' early); (2) gcc fills...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int EndOfHeap(void *);

extern int D_00241D08;
extern int D_00241D0C;
extern int D_00241D10;
extern int D_00241D14;

int func_001118B8(void) {
    int local[8];

    if (D_00241D08 != -1) {
        int v0 = D_00241D0C;
        if (v0 != -1)
            return v0;
    }
    local[5] = 0;
    local[1] = 1;
    local[2] = EndOfHeap(local);
    D_00241D08 = EndOfHeap(local);
    D_00241D0C = D_00241D08;
    local[2] = 0;
    D_00241D10 = EndOfHeap(local);
    D_00241D14 = 0;
    return 1;
}
