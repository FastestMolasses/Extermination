// NEARMISS func_001D91A0  (vram 0x001D91A0, 0x388 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.05% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order permutation (permuter territory): callee-saved coloring of the 4 saved args differs from target; all residual diffs follow from it. Body fully recovered.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS 94.05 (mwcc 2.3.3; pinned 991202 = 81.88). Body and control flow
// fully recovered and logically verified against the splat disassembly. The
// sole residual is a uniform callee-saved register-coloring permutation:
// the target colors {arg0:s3, arg1:s2, arg2:s1, arg3:s0} while mwcc colors
// {arg0:s0, arg1:s3, arg2:s2, arg3:s1}. Every remaining diff is a consequence
// (the swapped sN in each use, and the resulting commutative operand order in
// the RGBA-pack `or` chain). No readable-C reshape moves the coloring;
// register-allocation-order class -> permuter territory. Boot ELF stays
// byte-identical via the splat .s.
//
// GIF/DMA packet builder. Builds a GIFtag + draw packet into the display
// list head held at D_00275670[0x1C], zeroes the 0x70003400 scratch region,
// packs an RGBA color from arg1's 4 floats via func_00128250, transforms
// vertices via func_001026D0, then dispatches a draw via the func_001D* set
// (branch on arg3 selects variant).
typedef int u128 __attribute__((mode(TI)));

extern char *D_00275670;
extern u128 D_70003470[64];
extern u128 D_0026E540[64];
extern u128 D_70003440[64];
extern char D_70003400[256];
extern u128 D_70003AC0[64];

extern void func_001029C0(float *);
extern void func_001026D0(float *, float *, float *);
extern void func_001028B8(void *, char *, void *, char *);
extern int func_00128250(float);
extern void copy_qw4(char *, char *);
extern void func_001D1F80(int, int, int);
extern void func_001D4E20(int);
extern void func_001D4EB0(int);
extern void func_001D7080(int, int, float);
extern void func_001D4F30(int, int);
extern void func_001D4B80(int, int);
extern void func_001D4C30(int, int);

void func_001D91A0(float *arg0, char *arg1, int arg2, int arg3, float fp) {
    float sp60[16];
    float spA0[16];
    float spE0[16];
    float sp120[16];
    char *st;
    char *p;
    char *q;
    float scale;
    int color;

    scale = 0.1f * (fp / 20.0f);

    func_001029C0(spA0);
    spA0[0] = scale;
    spA0[5] = scale;
    spA0[10] = scale;
    func_001026D0(sp60, arg0, spA0);

    st = D_00275670;
    p = *(char **)(st + 0x1C);
    *(char *)(p + 3) = 0x10;
    *(int *)(*(char **)(st + 0x1C) + 4) = 0;
    *(short *)(*(char **)(st + 0x1C) + 0) = 5;
    q = *(char **)(st + 0x1C);
    *(char **)(st + 0x1C) = q + 0x60;

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

    func_001028B8(D_70003470, arg1, D_0026E540, p);

    *(int *)(q + 0x10) = 0x11000000;
    *(int *)(q + 0x14) = 0x01000101;
    *(int *)(q + 0x18) = 0;
    *(int *)(q + 0x1C) = 0x6C0403F5;
    *(u128 *)(q + 0x20) = D_70003440[0];
    *(u128 *)(q + 0x30) = D_70003440[1];
    *(u128 *)(q + 0x40) = D_70003440[2];
    *(u128 *)(q + 0x50) = D_70003440[3];

    st = D_00275670;
    p = *(char **)(st + 0x1C);
    *(char *)(p + 3) = 0x10;
    *(int *)(*(char **)(st + 0x1C) + 4) = 0;
    *(short *)(*(char **)(st + 0x1C) + 0) = 9;
    q = *(char **)(st + 0x1C);
    *(char **)(st + 0x1C) = q + 0xA0;

    *(u128 *)(q + 0x10) = 0;
    *(int *)(q + 0x14) = 0x01000101;
    *(int *)(q + 0x18) = 0;
    *(int *)(q + 0x1C) = 0x6C080000;

    func_001026D0((float *)(q + 0x20), (float *)D_70003AC0, sp60);
    func_001026D0((float *)(q + 0x60), (float *)D_70003400, sp60);

    color = (func_00128250(*(float *)(arg1 + 0xC)) << 24)
          | (func_00128250(*(float *)(arg1 + 8)) << 16)
          | (func_00128250(*(float *)(arg1 + 4)) << 8)
          | func_00128250(*(float *)(arg1 + 0));

    copy_qw4((char *)spE0, (char *)D_70003AC0);
    func_001026D0(sp120, (float *)(D_00275670 + 0x2380), sp60);
    func_001026D0((float *)D_70003AC0, (float *)(D_00275670 + 0x2340), sp120);

    if (arg3 == 0) {
        func_001D1F80(3, 2, 9);
        func_001D4E20(3);
        func_001D7080(3, color, 0.0f);
        func_001D4F30(3, arg2);
        func_001D4B80(3, arg2);
    } else {
        func_001D1F80(3, 2, 5);
        func_001D4EB0(3);
        func_001D7080(3, color, 0.0f);
        func_001D4F30(3, arg2);
        func_001D4C30(3, arg2);
    }

    copy_qw4((char *)D_70003AC0, (char *)spE0);
}
