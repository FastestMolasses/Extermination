// NEARMISS func_001A8F40  (vram 0x001A8F40, 0xBC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.21% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-coloring/scheduling permutation: target reloads *0x70003A20 early into $f1 and colors v into $f2 (mov.s/mul.s pairing); mwcc reloads it late and colors v into $f0, reordering the float-block stores/compare. Integer/setup half is byte-identical. Not the clean-store nop; not source-fixable. Perm...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS func_001A8F40 — readable decompilation, NOT byte-identical.
// objdiff 85.2% via mwcc 2.3.3 (mwcps2-2.3.3-000906); pinned 991202 reaches 78.8%.
// Logic and structure fully recovered; residual is a pure FP scheduling/coloring
// permutation no source change fixes here.
//
// Semantics: runs func_001028D0 over the two 0xB0 sub-blocks (arg1+0xB0 dst,
// arg0+0xB0 src) into scratch D_700038A0; zeroes the scalar at 0x700038AC; stores
// func_00102738(scratch,scratch) into the float at 0x70003A20. Then v = 3.0f +
// *(*(float**)(arg0+0x30)); stores v then v*v into 0x70003A24; if the 0x70003A20
// float <= v*v, writes the short 0x2014 to arg1+0x36 and zeroes the short at
// 0x70003B88.
//
// WALL (FP-coloring/scheduling permutation): the target reloads *0x70003A20 once,
// EARLY (into $f1 before the 3.0 mtc1), keeps it across the body, and computes v
// in $f2 (mov.s $f0,$f2 / mul.s $f0,$f2,$f0) so v survives for its own store. mwcc
// (both builds) schedules the *0x70003A20 reload LATE and colors v into $f0 / v*v
// into $f1 (add.s $f0,$f0,$f1 / mul.s $f1,$f0,$f0), reordering the tail stores and
// the compare. Every byte of the integer/setup half matches; only the float block
// schedule/coloring differs. Tried: lim-temp early read, comparing the stored
// 0x70003A24, sq temp, sdatathreshold variants — all <= 85.2%.
extern void func_001028D0(void *a, void *b, void *c);
extern float func_00102738(void *a, void *b);
extern char D_700038A0;

void func_001A8F40(char *arg0, char *arg1) {
    float v;

    func_001028D0(&D_700038A0, arg1 + 0xB0, arg0 + 0xB0);
    *(int *)0x700038AC = 0;
    *(float *)0x70003A20 = func_00102738(&D_700038A0, &D_700038A0);
    v = 3.0f + **(float **)(arg0 + 0x30);
    *(float *)0x70003A24 = v;
    *(float *)0x70003A24 = v * v;
    if (*(float *)0x70003A20 <= v * v) {
        *(short *)(arg1 + 0x36) = 0x2014;
        *(short *)0x70003B88 = 0;
    }
}
