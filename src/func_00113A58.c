// NEARMISS func_00113A58  (vram 0x00113A58, 0xF8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 74.68% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc frame-stride + global GPR-coloring wall (no permuter lever). Three independent factors: (1) frame-size stride -- target reserves -0x60, ours -0x50 (ra one slot higher); (2) global coloring -- target keeps the incoming arg in $s2 and the D_00241CF8 base pointer in $s3, ours uses $s1/$s2; (3) the uncached unalig...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_00112E28(int a);
extern void func_00122B58(char *s);
extern int func_0010E8A8(int a0,int a1,int a2,int a3,int t0,int t1,int t2,int t3,int s);
extern int CreateSema(int *p);
extern int D_00241CF8;
extern char D_0026BD88[];
extern char D_0026BDA8[];
extern int D_0027AB40[];
extern int D_0027AF60[];
extern int D_00241D0C;

struct u64 { long v; } __attribute__((packed));

int func_00113A58(char *s2) {
    int *s0 = D_0027AB40;

    if (func_00112E28(0xF) == 0) {
        return 0;
    }
    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BD88);
    }
    if (func_0010E8A8((int)D_0027AF60, 1, 0, 0, 0, (int)s0, 0x10, 0, 0) < 0) {
        CreateSema((int *)D_00241D0C);
        return 0;
    } else {
        struct u64 *src = (struct u64 *)((char *)((long)s0 | 0x20000000) + 4);
        ((struct u64 *)s2)->v = src->v;
        if (D_00241CF8 > 0) {
            func_00122B58(D_0026BDA8);
        }
        CreateSema((int *)D_00241D0C);
        return *(int *)((long)s0 | 0x20000000);
    }
}
