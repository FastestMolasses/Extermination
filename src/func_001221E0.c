// NEARMISS func_001221E0  (vram 0x001221E0, 0x100 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.08% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// ee-gcc register-allocation permutation (NOT mwcc; target uses sd/ld/daddu EABI saves -> compiled with ee-gcc 2.9 -O2, which lifted it from 0.5% to 87%). Logic fully recovered, instruction stream matches exactly. Sole residual: ee-gcc colors this dataflow with 5 saved regs (frame 0x60) vs target's...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// Binary-doubling list/accumulator builder. Low 2 bits of arg2 select an
// initial node via func_00121BD0(a0,a1,D_0026C808[bits-1],0); then for each set
// bit of (arg2>>2) it either grafts the running result onto a freshly built
// node (func_00121FB0 + func_00121BA0) or walks/extends the chain anchored at
// *(a0+0x48), allocating fresh links (func_00121F78 / func_00121FB0) as needed.
extern int func_00121BA0(unsigned char *, int *);
extern int *func_00121BD0(unsigned char *, int *, int, int);
extern int *func_00121F78(unsigned char *, int);
extern int *func_00121FB0(unsigned char *, int *, int *);
extern int D_0026C808[];

int *func_001221E0(unsigned char *a0, int *a1, int arg2) {
    int *s2;
    int *s0;
    int *v0;
    int s1;

    s2 = a1;
    if ((arg2 & 3) != 0) {
        s2 = func_00121BD0(a0, a1, D_0026C808[(arg2 & 3) - 1], 0);
    }
    s1 = arg2 >> 2;
    v0 = s2;
    if (s1 != 0) {
        s0 = *(int **)(a0 + 0x48);
        if (s0 == 0) {
            s0 = func_00121F78(a0, 0x271);
            *(int **)(a0 + 0x48) = s0;
            *s0 = 0;
        } else {
            goto have_node;
        }
        for (;;) {
        have_node:
            if (s1 & 1) {
                v0 = func_00121FB0(a0, s2, s0);
                func_00121BA0(a0, s2);
                s2 = v0;
            }
            s1 = s1 >> 1;
            if (s1 == 0) {
                break;
            }
            v0 = (int *)*s0;
            if (v0 == 0) {
                v0 = func_00121FB0(a0, s0, s0);
                *s0 = (int)v0;
                *v0 = 0;
            }
            s0 = v0;
        }
        v0 = s2;
    }
    return v0;
}
