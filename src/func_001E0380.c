// NEARMISS func_001E0380  (vram 0x001E0380, 0x158 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.71% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-register coloring + scheduling permutation. After fixing frame size (sp90 is a 0x40-byte workspace, sp80 a 16-byte quadword, ordered so sp80=0x80/sp90=0x90), integer reg coloring (declare loop counters i/j before the pointers), and the preamble statement order (qword copy + s2/s3 before the an...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Builds two 16x16 grids of transforms (bases D_0081C0F0 and D_0081D0F0, each
// row stride 0x100, each cell 0x10). For each (i,j) in [0,16): seeds a 0x40-byte
// workspace `sp90` via func_001029C0, rotates it about one axis by
// (PI/2 * i)/15 (func_00102A60) and about another by ((2*PI * j)/15) - PI
// (func_00102BB0), then func_001026A0 composes it with the per-cell base and the
// constant quadword D_00253520 (copied into `sp80`) into both grids.
//
// NEARMISS (mwcc233 94.7%, 991202 73.5%): frame layout, integer register
// coloring, and loop body are byte-identical. Sole residual is FP odd-half
// register coloring of the (float)i path plus quadword-copy register choice and
// the `sq` scheduling slot in the loop preamble -- permuter-class artifacts.
typedef struct { int a, b, c, d; } qw;
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001029C0(void *p);
extern void func_00102A60(void *a, void *b, float f);
extern void func_00102BB0(void *a, void *b, float f);
extern qw D_00253520;
extern char D_0081C0F0;
extern char D_0081D0F0;

void func_001E0380(void) {
    qw sp80;
    char sp90[0x40];
    int i;
    int j;
    char *s2;
    char *s3;
    char *s4;
    char *s5;

    s4 = &D_0081C0F0;
    s5 = &D_0081D0F0;
    for (i = 0; i < 0x10; i++) {
        float ang;
        sp80 = D_00253520;
        s2 = s4;
        s3 = s5;
        ang = (1.5707964f * (float)i) / 15.0f;
        for (j = 0; j < 0x10; j++) {
            func_001029C0(sp90);
            func_00102A60(sp90, sp90, ang);
            func_00102BB0(sp90, sp90, ((6.2831855f * (float)j) / 15.0f) - 3.1415927f);
            func_001026A0(s2, sp90, &sp80);
            func_001026A0(s3, sp90, &sp80);
            s2 += 0x10;
            s3 += 0x10;
        }
        s4 += 0x100;
        s5 += 0x100;
    }
}
