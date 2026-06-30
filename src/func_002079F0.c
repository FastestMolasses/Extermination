// NEARMISS func_002079F0  (vram 0x002079F0, 0x1B4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.73% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring + branch-sense permutation at 98.73% (mwcc 2.3.3). Body fully correct. Residuals: (a) the two table base pointers (D_00264FE0/FE2) color into swapped v0/v1 and the *4 index into a0 vs target a2; (b) the span = idx<0xA?0x20:0x40 select compiles to beqz where target uses bnez (inv...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 98.73% (mwcc 2.3.3; mwcc 991202 = 76.51%). Body/structure fully
// recovered: five func_00207E40 setup calls, then an index from *(s16*)(arg0+0x28)
// drives two D_00264FE0/FE2 table lookups fed through float_to_int (16.0f scale,
// +0x700 / >>1 +0x790), a span = idx<10?0x20:0x40 select (sign-extended to the
// long-long 4th arg), and a final dispatch via the D_00265010[idx] row index.
// Residuals are pure regalloc/scheduling: the two table-base pointers color into
// swapped v0/v1 (and the index into a0 vs a2), and the span branch is emitted with
// inverted sense (beqz vs target bnez). Body identical. Permuter territory.
extern void func_00207E40(int a0, int a1, int a2, long long a3, int t0, unsigned int t1, long long t2);
extern int float_to_int(float);
extern short D_00264FE0;
extern short D_00264FE2;
extern unsigned char D_00265010;

void func_002079F0(char *arg0, char *arg1) {
    int idx;
    short e0;
    short e2;
    int a2v;
    int a3v;
    short span;
    int row;

    func_00207E40(1, 0x7000, 0x7900, 0x100, 0xE0, 0x80808080, *(long long *)(arg1 + 0x0));
    func_00207E40(1, 0x8000, 0x7900, 0x100, 0xE0, 0x80808080, *(long long *)(arg1 + 0x8));
    func_00207E40(1, 0x7000, 0x8000, 0x100, 0xE0, 0x80808080, *(long long *)(arg1 + 0x10));
    func_00207E40(1, 0x8000, 0x8000, 0x100, 0xE0, 0x80808080, *(long long *)(arg1 + 0x18));
    func_00207E40(1, 0x8570, 0x7E60, 0x80, 0x80, 0x80808080, *(long long *)(arg1 + 0x90));

    idx = *(short *)(arg0 + 0x28);
    e0 = *(short *)((char *)&D_00264FE0 + idx * 4);
    e2 = *(short *)((char *)&D_00264FE2 + idx * 4);
    a2v = float_to_int(16.0f * (float)(e0 + 0x700));
    a3v = float_to_int(16.0f * (float)((e2 >> 1) + 0x790));
    span = (idx < 0xA) ? 0x20 : 0x40;
    row = *(unsigned char *)((char *)&D_00265010 + idx);
    func_00207E40(1, a2v, a3v, span, 0x20, 0x80808080,
                  *(long long *)((row << 3) + arg1 + 0x20));
}
