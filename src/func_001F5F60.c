// NEARMISS func_001F5F60  (vram 0x001F5F60, 0x2AC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.65% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation/scheduling permutation only. Body and control flow are byte-for-byte structurally identical to target (verified instruction-by-instruction): a display-list cursor builder (D_00275670 + 0x1C) that zeroes a 0x70003400 quadword-copy VU0 scratch block, copies a 4x u128 matrix from...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Display-list / VU0-scratch record builder. Snapshots the cursor object at
// D_00275670+0x1C into `s0` for later use, builds two local transforms (sp90
// via func_00102948, sp50 via func_001029C0/func_00102C58/func_00102918 using
// arg1), then writes a type=5 (0x10-byte) cursor record, zero-fills the whole
// 0x70003400..0x7000346C VU0 scratch quadword block, and calls func_001028B8
// with the composed args (D_70003470, arg2, D_0026EB50, cursor-record ptr).
// Writes a second inline matrix-ish record at s1 (header words + a 4x
// interleaved-lq/sq copy of the 0x40-byte block at D_70003440), then a third
// type=9 (0xA0-byte) cursor record whose body is built from func_001026D0
// twice (once against D_70003AC0, once against D_70003400) using sp50 as the
// working matrix, followed by func_001D3990(arg3). Finally writes a fourth
// type=0 (0x10-byte) trailer record and hands the whole thing off to
// func_001CAAC0(sp90, s0, D_00275670).
//
// NEARMISS 93.65% with mwcc 2.3.3 (mwcps2-2.3.3-000906). Keys applied:
// (1) D_00275670 is a gp-rel pointer global (-sdatathreshold 8 required);
// D_0026EB50/D_70003400/D_70003440/D_70003470/D_70003AC0 are all in the
// 0x7000xxxx VU0-scratch overlay far outside gp range, so each is
// over-declared as int[4] to force absolute lui/%hi %lo addressing at that
// same threshold (array-over-declaration idiom). (2) The 0x40-byte block
// copy from D_70003440 uses the interleaved-lq/sq idiom via a
// `typedef int u128 __attribute__((mode(TI)))`. (3) The base pointer
// D_00275670 is cached into a local once per call-free span (matching the
// target's $t1 reuse across a JAL-free instruction run) rather than
// re-dereferenced per access. (4) The delayed `sp90[3]=1.0f` word write uses
// a `struct{float x,y,z; int w;}` local instead of `float[4]`+int-cast, per
// the mixed-type vec4 stack buffer idiom, to reproduce the direct
// `sw v0,0xc(sp)` instead of an extra pointer-materializing `addiu`.
// Residual: pure register-coloring/scheduling permutation -- parked for the
// permuter pass.

extern void func_00102948(void *, void *);
extern void func_001029C0(void *);
extern void func_00102C58(void *, void *, void *);
extern void func_00102918(void *, void *, void *);
extern void func_001028B8(void *, int, void *, void *);
extern void func_001026D0();
extern void func_001D3990(int);
extern void func_001CAAC0(void *, void *, void *);

typedef int u128 __attribute__((mode(TI)));

extern char *D_00275670;
extern int D_0026EB50[4];
extern int D_70003400[4];
extern int D_70003440[4];
extern int D_70003470[4];
extern int D_70003AC0[4];

void func_001F5F60(void *arg0, int arg1, int arg2, int arg3) {
    u128 sp50[4];
    struct { float x, y, z; int w; } sp90;
    char *s0;
    char *s1;
    char *v0;
    char *a3;
    char *base;

    s0 = *(char **)(D_00275670 + 0x1C);

    func_00102948(&sp90, arg0);
    sp90.w = 0x3F800000;
    func_001029C0(sp50);
    func_00102C58(sp50, sp50, (void *)arg1);
    func_00102918(sp50, sp50, &sp90);

    base = D_00275670;
    a3 = *(char **)(base + 0x1C);
    *(char *)(a3 + 3) = 0x10;
    *(int *)(*(char **)(base + 0x1C) + 4) = 0;
    *(short *)(*(char **)(base + 0x1C) + 0) = 5;
    v0 = *(char **)(base + 0x1C);
    s1 = v0 + 0x10;
    *(char **)(base + 0x1C) = v0 + 0x60;

    *(int *)0x70003400 = 0;
    *(int *)0x70003404 = 0;
    *(int *)0x70003408 = 0;
    *(int *)0x7000340C = 0;
    *(int *)0x70003410 = 0;
    *(int *)0x70003414 = 0;
    *(int *)0x70003418 = 0;
    *(int *)0x7000341C = 0;
    *(int *)0x70003420 = 0;
    *(int *)0x70003424 = 0;
    *(int *)0x70003428 = 0;
    *(int *)0x7000342C = 0;
    *(int *)0x70003430 = 0;
    *(int *)0x70003434 = 0;
    *(int *)0x70003438 = 0;
    *(int *)0x7000343C = 0;
    *(int *)0x70003440 = 0;
    *(int *)0x70003444 = 0;
    *(int *)0x70003448 = 0;
    *(int *)0x7000344C = 0;
    *(int *)0x70003450 = 0;
    *(int *)0x70003454 = 0;
    *(int *)0x70003458 = 0;
    *(int *)0x7000345C = 0;
    *(int *)0x70003460 = 0;
    *(int *)0x70003464 = 0;
    *(int *)0x70003468 = 0;
    *(int *)0x7000346C = 0;

    func_001028B8(D_70003470, arg2, D_0026EB50, a3);

    *(int *)(s1 + 0x00) = 0x11000000;
    *(int *)(s1 + 0x04) = 0x01000101;
    *(int *)(s1 + 0x08) = 0;
    *(int *)(s1 + 0x0C) = 0x6C0403F5;
    v0 = (char *)D_70003440;
    *(u128 *)(s1 + 0x10) = *(u128 *)(v0 + 0x00);
    *(u128 *)(s1 + 0x20) = *(u128 *)(v0 + 0x10);
    *(u128 *)(s1 + 0x30) = *(u128 *)(v0 + 0x20);
    *(u128 *)(s1 + 0x40) = *(u128 *)(v0 + 0x30);

    base = D_00275670;
    a3 = *(char **)(base + 0x1C);
    *(char *)(a3 + 3) = 0x10;
    *(int *)(*(char **)(base + 0x1C) + 4) = 0;
    *(short *)(*(char **)(base + 0x1C) + 0) = 9;
    a3 = *(char **)(base + 0x1C);
    *(char **)(base + 0x1C) = a3 + 0xA0;

    *(u128 *)(a3 + 0x10) = 0;
    *(int *)(a3 + 0x14) = 0x01000101;
    s1 = a3 + 0x10;
    *(int *)(a3 + 0x1C) = 0x6C080000;

    func_001026D0(s1 + 0x10, D_70003AC0, sp50, a3);
    func_001026D0(s1 + 0x50, D_70003400, sp50);
    func_001D3990(arg3);

    base = D_00275670;
    a3 = *(char **)(base + 0x1C);
    *(char *)(a3 + 3) = 0x60;
    *(int *)(*(char **)(base + 0x1C) + 4) = 0;
    *(short *)(*(char **)(base + 0x1C) + 0) = 0;
    v0 = *(char **)(base + 0x1C);
    v0 = v0 + 0x10;
    *(char **)(base + 0x1C) = v0;

    func_001CAAC0(&sp90, s0, D_00275670);
}
