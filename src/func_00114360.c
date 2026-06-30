// NEARMISS func_00114360  (vram 0x00114360, 228 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.82% via ee-gcc 2.9-991111-01 (-O2; reproduce: the // COMPILER/CFLAGS below).
// DIVERGENCE (compiler artifact, not a logic error): deterministic GPR register coloring.
// The original keeps the `st = &D_0027B184 - 0x14` pointer in $v1 (and reuses it as the tail
// return-value register); ee-gcc 2.9 colors it into $v0, which cascades register naming through
// the body. No source shape flips ee-gcc's allocator here (5 restructurings tried; a decomp-
// permuter run reaches ~98.4% but only via mechanical temp/decl shuffles that mangle readability).
// The LOGIC and STRUCTURE are faithful — only register naming differs.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT from this C
// (fill_unmatched treats `// NEARMISS` like a stub). This file is faithful ground truth for the
// port, not a compiled unit, and is excluded from the matched_code metric. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241D68;
extern char D_0027B0C0;
extern char D_0027B184;
extern char D_0027C680;

extern int func_00123418(void *a0, int a1, int a2);
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int t0, void *t1, int t2, int t3, int sp0);

int func_00114360(int arg0, int arg1, int arg2, int arg3)
{
    int r;
    char *base;
    char *st;

    if (D_00241D68 != 0)
        return D_00241D68;

    base = &D_0027B0C0;
    if (*(int *)(base + 0x24) == 0)
        return -100;

    func_00123418(&D_0027B184, arg2, 0x3FF);

    st = &D_0027B184 - 0x14;
    *(int *)st = arg0;
    *(int *)(st + 8) = arg3;
    *(int *)(st + 4) = arg1;
    *(char *)(st + 0x413) = 0;

    r = func_0010E8A8(base, 2, 1, st, 0x414, &D_0027C680, 4, 0, 0);
    if (r != 0)
        return r;

    D_00241D68 = 2;
    return r;
}
