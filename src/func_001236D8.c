// NEARMISS func_001236D8  (vram 0x001236D8, 0x78 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.83% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// ee-gcc function (target uses sd/ld 64-bit callee saves, daddu reg moves, and a bnezl branch-likely -> mwcc 2.3.3/991202 emit sq/lq + paddub and only reach 56.3%; this is ee-gcc codegen, a known wall mwcc cannot fix). Best compiler is ee-gcc 2.9-991111-01 -O2 at 87.8% with the LOGIC FULLY RECOVERE...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// NEARMISS 87.8% (ee-gcc 2.9-991111-01 -O2). Logic fully recovered.
// Residuals are pure ee-gcc artifacts: callee-save coloring of the two
// surviving params is swapped, and ee-gcc sibling-call-optimizes the tail
// void call ('j sub__...Inf') where the target keeps the frame ('jal' +
// restore + 'jr ra'). 2.3.3/991202 cannot help (ee-gcc codegen wall).
//
// Lazily binds the entity's handler at +0x54 to the global default
// D_0024295C, one-time-inits it via func_00120058 if its +0x38 flag is
// clear (branch-likely guard), then dispatches sub__0000000000000000Inf
// with the handler and the (self, a1, a2) call context.
extern int D_0024295C;
extern void func_00120058(int);
extern void sub__0000000000000000Inf(int, int, int, int);

void func_001236D8(int a0, int a1, int a2) {
    int v1;

    v1 = *(int *)(a0 + 0x54);
    if (v1 == 0) {
        v1 = D_0024295C;
        *(int *)(a0 + 0x54) = v1;
    }
    if (*(int *)(v1 + 0x38) == 0) {
        func_00120058(v1);
    }
    sub__0000000000000000Inf(*(int *)(a0 + 0x54), a0, a1, a2);
}
