// NEARMISS func_0010A248  (vram 0x0010A248, 0x4C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 57.26% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// ee-gcc list-scheduler permutation wall. Reg-alloc and all instructions match the target exactly (verified via objdiff alignment); the two residual differences are pure scheduling: (1) the epilogue `ld ra` is hoisted earlier in target than gcc places it for our source, and (2) the `sw zero,0x280(v1)` store is the 2nd...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned short func_00105148(int);
extern unsigned int D_002412E8;
extern unsigned int D_002418F8;

void func_0010A248(void) {
    unsigned int v0;
    unsigned int a, b, c;
    char *base;
    func_00105148(1);
    v0 = D_002412E8;
    base = (char *)(&D_002418F8);
    a = v0 + 0x3300;
    do { b = v0 + 0x1800; c = v0 + 0x1B00; } while (0);
    *((unsigned int *)(base + 0x0)) = v0;
    *((unsigned int *)(base + 0x280)) = 0;
    *((unsigned int *)(base + 0x4)) = b;
    *((unsigned int *)(base + 0x140)) = c;
    *((unsigned int *)(base + 0x144)) = a;
}
