// NEARMISS func_001066F8  (vram 0x001066F8, 0x134 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.25% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc GPR-coloring wall (confirmed). Body/structure/opcodes all correct; the only residual is that expected colors the IPU_CMD constant 0x40000000 into $a1 (`lui a1,0x4000`, twice — once per predecessor block) while ee-gcc 2.9-991111-01 colors it into $a0. That steals $a0 from the &D_00241170 arg...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_00109B70(int);
extern long func_00106490(int *);
extern int D_002412F4;
extern int D_00241324;
extern int D_00241170[];
extern int D_00241B98;
extern int D_00241B9C;

int func_001066F8(int n) {
    int count;

    if (D_00241324 != 0 || D_00241B9C < n) {
        count = 0;
        while ((*(volatile unsigned int *)0x10002010 & 0x80004000) == 0x80000000) {
            if (count++ >= 0x1389) {
                func_00109B70(D_002412F4);
                count = 0;
            }
        }
        *(volatile unsigned int *)0x10002000 = 0x40000000;
        D_00241324 = D_00241170[4];
        D_00241B98 = (int)func_00106490(D_00241170);
        D_00241B9C = 32;
    }
    return (unsigned int)D_00241B98 >> (32 - n);
}
