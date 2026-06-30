// NEARMISS func_00108DB0  (vram 0x00108DB0, 0xF4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.23% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc GPR-coloring wall (no permuter lever, confirmed s84). Body is structurally identical after fixing the gcc loop-peel (single for-loop from i=0 over a 0x18-byte entry struct removes the peeled first iteration). Sole residual: ee-gcc colors the struct pointer (a0[0x40]) into $s1 and the result accumulator into $s...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_00108818(int a);
extern long D_00241238[];

struct ent { long key; long f8; int f10; int f14; };

int func_00108DB0(int *a0, int a1, int a2, int a3, int t0) {
    int s1 = 0;
    int *s2 = *(int **)((char *)a0 + 0x40);
    struct ent *s0 = *(struct ent **)((char *)s2 + 0x44);
    long key = func_00108818(a1);
    int cnt = *(int *)((char *)s2 + 0x48);
    int i = 0;

    for (i = 0; i < cnt; i++) {
        if (key == s0[i].key) {
            s1 = s0[i].f10;
            break;
        }
    }

    if (i < 0x40) {
        struct ent *e = &s0[i];
        long *tbl = (long *)((char *)D_00241238 + (a1 << 4));
        *(int *)((char *)s2 + 0x48) = cnt + 1;
        e->key = key;
        e->f14 = t0;
        e->f10 = a3;
        e->f8 = tbl[1];
    }
    return s1;
}
