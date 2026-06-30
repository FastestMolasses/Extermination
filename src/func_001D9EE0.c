// NEARMISS func_001D9EE0  (vram 0x001D9EE0, 0x194 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.75% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 99.75% mwcc 2.3.3 (-sdatathreshold 4). Body byte-identical; sole residual is a register-coloring swap ($7<->$8) in the first 3 prologue instructions (boot: lw $7=dl / li $8=0x30 / sb $8,3($7); mwcc: lw $8 / li $7 / sb $7,3($8)). Register-allocation-order -> permuter.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Builds a GS/DMA tag header into the display-list cursor (D_00275670 word
// 0x10: sets byte+3=0x30, word+4=&D_00817E20, half+0=0x10, advances cursor by
// 0x10), then composes two scratch matrices in the uncached buffer D_70003AC0:
// one seeded from a template (D_00817F20) with the float arg on the diagonal
// (sp+0x70 elems 0,5) and 1.0 at elems 10,15, the other a fixed projection-ish
// matrix (sp+0xB0), each run through func_001026D0. Then it snapshots self+0x80
// via func_00102948, zeroes self+0x80..0x88, runs func_001C7420(self,0x3F5,0),
// restores self+0x80, kicks func_001D4740(D_0028A490[*(short*)(self+0x96)]),
// copies the first matrix back, and calls func_001D1F20(0).
//
// NEARMISS 99.75% with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202
// build reaches only 79.9% (worse list-scheduling of the prologue). Keys:
// (1) D_00275670 is a gp-rel pointer global so -sdatathreshold 4 is required,
// while D_00817E20/D_00817F20/D_70003AC0 must be declared as arrays so they
// stay absolute (hi/lo) at that threshold; (2) the matrices are a
// union{float;int}[16] struct so field stores fold to direct off(sp) rather
// than materializing each base with addiu. Sole residual: a register-coloring
// swap ($7<->$8) in the first three instructions -- the target colors the
// dl-cursor load $7 and the 0x30 literal $8; mwcc 2.3.3 picks the opposite
// (lw $8 / li $7 / sb $7,3($8) vs boot lw $7 / li $8 / sb $8,3($7)). Body
// byte-identical otherwise -> register-allocation-order permuter territory.
typedef struct { int dl; } DLState;
extern char *D_00275670;         // gp-rel: word 0x10 holds the display-list cursor
extern int D_00817E20[16];
extern int D_00817F20[16];
extern char D_70003AC0[64];      // uncached scratch matrix buffer
extern int D_0028A490[];

extern void copy_qw4(void *dst, void *src);
extern void func_001029C0(void *m);
extern void func_001026D0(void *dst, void *a, void *b);
extern void func_00102948(void *dst, void *src);
extern void func_001C7420(int a, int b, int c);
extern void func_001D4740(int a);
extern void func_001D1F20(int a);

typedef struct { union { float f; int i; } e[16]; } Mtx;

void func_001D9EE0(float arg0, char *arg1) {
    float farg;
    int dl;
    char sp30[0x40];   // sp+0x30
    Mtx sp70;          // sp+0x70
    Mtx spB0;          // sp+0xB0
    char spF0[0x10];   // sp+0xF0
    char *self = arg1;
    char *st = (char *)D_00275670;

    farg = arg0;

    dl = *(int *)(st + 0x10);
    *(char *)(dl + 3) = 0x30;
    dl = *(int *)(st + 0x10);
    *(int *)(dl + 4) = (int)D_00817E20;
    dl = *(int *)(st + 0x10);
    *(short *)dl = 0x10;
    *(int *)(st + 0x10) = *(int *)(st + 0x10) + 0x10;

    copy_qw4(sp30, D_70003AC0);
    copy_qw4(D_70003AC0, D_00817F20);
    func_001029C0(&sp70);

    sp70.e[5].f = farg;
    sp70.e[0].f = farg;
    sp70.e[10].i = 0x3F800000;
    sp70.e[15].i = 0x3F800000;
    func_001026D0(D_70003AC0, &sp70, D_70003AC0);

    spB0.e[0].i  = 0x3F800000;
    spB0.e[4].i  = 0;
    spB0.e[8].i  = 0;
    spB0.e[12].i = 0x45000000;
    spB0.e[1].i  = 0;
    spB0.e[5].i  = 0x3F800000;
    spB0.e[9].i  = 0;
    spB0.e[13].i = 0x45000000;
    spB0.e[2].i  = 0;
    spB0.e[6].i  = 0;
    spB0.e[10].i = 0x3F664CB3;
    spB0.e[14].i = 0x49CCCCCC;
    spB0.e[3].i  = 0;
    spB0.e[7].i  = 0;
    spB0.e[11].i = 0;
    spB0.e[15].i = 0x3F800000;
    func_001026D0(D_70003AC0, &spB0, D_70003AC0);

    func_00102948(spF0, self + 0x80);
    *(int *)(self + 0x80) = 0;
    *(int *)(self + 0x84) = 0;
    *(int *)(self + 0x88) = 0;
    func_001C7420((int)self, 0x3F5, 0);
    func_00102948(self + 0x80, spF0);
    func_001D4740(D_0028A490[*(short *)(self + 0x96)]);
    copy_qw4(D_70003AC0, sp30);
    func_001D1F20(0);
}
