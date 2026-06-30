// NEARMISS func_001DA310  (vram 0x001DA310, 0x384 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.21% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation/scheduling permutation (permuter territory). Body+control flow fully recovered and logically verified. The 128-bit *arg0 qword copy (lq/sq), the gp-rel D_00275670[0x10] access, and the absolute 0x70003400 stores match. Residuals: (1) callee-saved sN coloring of the saved args ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS 93.21 (mwcc 2.3.3; pinned 991202 = 82.05). Body and control flow
// fully recovered and logically verified against the splat disassembly. The
// residual is a register-coloring/scheduling permutation: callee-saved sN
// assignment for the saved args differs from the target, and the
// %hi(D_70003440) base is rematerialized per qword copy instead of held in one
// register (target keeps t2). Both are register-allocation/scheduling artifacts
// no readable-C reshape fixes -> permuter territory. Boot ELF stays
// byte-identical via the splat .s.
//
// GIF/DMA packet builder (sibling of func_001D91A0). Builds a scaled transform
// from arg0's qword and fparg0, composes it via func_00102918/func_00102C58,
// writes a GIFtag + draw packet into the display-list head at D_00275670[0x10],
// zeroes the 0x70003400 scratch region, packs an RGBA color from arg2's 4
// floats via func_00128250, then dispatches the draw (func_001D1F80/...).
typedef int u128 __attribute__((mode(TI)));

extern char *D_00275670;
extern u128 D_70003470[64];
extern u128 D_0026E610[64];
extern u128 D_70003440[64];
extern char D_70003400[256];
extern u128 D_70003AC0[64];

extern void func_001029C0(float *);
extern void func_001026D0(float *, float *, float *);
extern void func_001028B8(void *, char *, void *, char *);
extern void func_00102918(float *, float *, int *);
extern void func_00102C58(float *, float *, int);
extern int func_00128250(float);
extern void copy_qw4(char *, char *);
extern void func_001D1F80(int, int, int);
extern void func_001D4EA0(void);
extern void func_001D7080(int, int, float);
extern void func_001D4FB0(int);
extern void func_001D4C20(int);

void func_001DA310(u128 *arg0, int arg1, char *arg2, int arg3, float fp) {
    float sp50[16];
    float sp90[16];
    u128 spD0;
    int spE0[4];
    float spF0[16];
    float sp130[16];
    char *st;
    char *p;
    char *q;
    u128 *m;
    float scale;
    int color;

    spD0 = *arg0;
    *(int *)((char *)&spD0 + 0xC) = 0x3F800000;

    func_001029C0(sp50);
    func_001029C0(sp90);
    scale = 0.1f * fp;
    sp90[0] = scale;
    *(int *)(sp90 + 5) = 0x40800000;
    sp90[10] = scale;
    func_001026D0(sp50, sp90, sp50);

    spE0[0] = 0;
    spE0[1] = 0xC1A00000;
    spE0[2] = 0;
    spE0[3] = 0x3F800000;
    func_00102918(sp50, sp50, spE0);
    func_00102C58(sp50, sp50, arg1);
    func_00102918(sp50, sp50, (int *)&spD0);

    st = D_00275670;
    p = *(char **)(st + 0x10);
    *(char *)(p + 3) = 0x10;
    *(int *)(*(char **)(st + 0x10) + 4) = 0;
    *(short *)(*(char **)(st + 0x10) + 0) = 5;
    q = *(char **)(st + 0x10);
    *(char **)(st + 0x10) = q + 0x60;

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

    func_001028B8(D_70003470, arg2, D_0026E610, p);

    *(int *)(q + 0x10) = 0x11000000;
    *(int *)(q + 0x14) = 0x01000101;
    *(int *)(q + 0x18) = 0;
    *(int *)(q + 0x1C) = 0x6C0403F5;
    m = D_70003440;
    *(u128 *)(q + 0x20) = m[0];
    *(u128 *)(q + 0x30) = m[1];
    *(u128 *)(q + 0x40) = m[2];
    *(u128 *)(q + 0x50) = m[3];

    st = D_00275670;
    p = *(char **)(st + 0x10);
    *(char *)(p + 3) = 0x10;
    *(int *)(*(char **)(st + 0x10) + 4) = 0;
    *(short *)(*(char **)(st + 0x10) + 0) = 9;
    q = *(char **)(st + 0x10);
    *(char **)(st + 0x10) = q + 0xA0;

    *(u128 *)(q + 0x10) = 0;
    *(int *)(q + 0x14) = 0x01000101;
    *(int *)(q + 0x18) = 0;
    *(int *)(q + 0x1C) = 0x6C080000;

    func_001026D0((float *)(q + 0x20), (float *)D_70003AC0, sp50);
    func_001026D0((float *)(q + 0x60), (float *)D_70003400, sp50);

    color = (func_00128250(*(float *)(arg2 + 0xC)) << 24)
          | (func_00128250(*(float *)(arg2 + 8)) << 16)
          | (func_00128250(*(float *)(arg2 + 4)) << 8)
          | func_00128250(*(float *)(arg2 + 0));

    copy_qw4((char *)spF0, (char *)D_70003AC0);
    func_001026D0(sp130, (float *)(D_00275670 + 0x2380), sp50);
    func_001026D0((float *)D_70003AC0, (float *)(D_00275670 + 0x2340), sp130);

    func_001D1F80(0, 2, 9);
    func_001D4EA0();
    func_001D7080(0, color, 0.0f);
    func_001D4FB0(arg3);
    func_001D4C20(arg3);

    copy_qw4((char *)D_70003AC0, (char *)spF0);
}
