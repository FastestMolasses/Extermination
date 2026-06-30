// NEARMISS func_00110608  (vram 0x00110608, 0x84 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 60.70% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// ee-gcc reg-alloc + frame-size wall. Correct-frame (0x110) C reaches objdiff 86.3% (permuter score 510). All instructions present and structure identical; remaining diff is register allocation of the two multiply products (which reg holds 0x60 vs 0x180 / a1 vs a3) and placement of the `addiu a1,zero,0x1` constant for...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern char D_00279750[];
extern int iGsPutIMR(void *arg, int n);
extern void func_00122B58(void *p);
extern char D_0026B9E0[];

void func_00110608(int a0, int a1) {
    int local[0x100 / 4];
    char *e = D_00279750 + a1 * 0x60 + a0 * 0x180;

    local[2] = 0x14;
    local[3] = 0;
    *(int *)(e + 0x20) = *(int *)(e + 0x20) + 1;
    local[0] = (int)(e + 0x20);
    local[1] = *(int *)(e + 0x10);
    if (iGsPutIMR(local, 1) == 0) {
        func_00122B58(D_0026B9E0);
    }
}
