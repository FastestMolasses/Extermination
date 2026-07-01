// NEARMISS func_001D30A0  (vram 0x001D30A0, 0x724 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.08% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Bulk per-slot-array initializer (dynamic index shift = D_00275670->0x9c << 7, sibling of matched func_001D2E20/func_001D38A0). Body/logic fully recovered: 12 base arrays each get a 3x func_00102948 quadword-copy at +0x60/+0x70/+0x50 from ctx+0x2220/0x2230/0xA0; D_00816B98/B9C/C98/C9C get inline f...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern void func_00102948(void *dst, void *src);

extern char *D_00275670;
extern char D_00816440[0x10000];
extern char D_00816540[0x10000];
extern char D_00816640[0x10000];
extern char D_00816740[0x10000];
extern char D_00816840[0x10000];
extern char D_00816940[0x10000];
extern char D_00816A40[0x10000];
extern char D_00816B40[0x10000];
extern char D_00816C40[0x10000];
extern char D_00816D40[0x10000];
extern char D_00816E40[0x10000];
extern char D_00816F40[0x10000];
extern char D_00817040[0x10000];
extern char D_00817140[0x10000];

extern int D_00816B98[2];
extern int D_00816B9C[2];
extern int D_00816C98[2];
extern int D_00816C9C[2];

extern int D_008168A0[2];
extern int D_008168A4[2];
extern int D_008168A8[2];
extern int D_008168AC[2];
extern int D_008168B0[2];
extern int D_008168B4[2];
extern int D_008168B8[2];
extern int D_008168BC[2];

typedef int u128 __attribute__((mode(TI)));

extern u128 D_002513E0[4];
extern u128 D_70003AC0[4];

void func_001D30A0(void) {
    func_00102948(D_00816440 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x60, D_00275670 + 0x2220);
    func_00102948(D_00816440 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x70, D_00275670 + 0x2230);
    func_00102948(D_00816440 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x50, D_00275670 + 0xA0);

    func_00102948(D_00816640 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x60, D_00275670 + 0x2220);
    func_00102948(D_00816640 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x70, D_00275670 + 0x2230);
    func_00102948(D_00816640 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x50, D_00275670 + 0xA0);

    func_00102948(D_00816740 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x60, D_00275670 + 0x2220);
    func_00102948(D_00816740 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x70, D_00275670 + 0x2230);
    func_00102948(D_00816740 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x50, D_00275670 + 0xA0);

    func_00102948(D_00816B40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x60, D_00275670 + 0x2220);
    func_00102948(D_00816B40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x70, D_00275670 + 0x2230);
    func_00102948(D_00816B40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x50, D_00275670 + 0xA0);

    func_00102948(D_00816C40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x60, D_00275670 + 0x2220);
    func_00102948(D_00816C40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x70, D_00275670 + 0x2230);
    func_00102948(D_00816C40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x50, D_00275670 + 0xA0);

    func_00102948(D_00816D40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x60, D_00275670 + 0x2220);
    func_00102948(D_00816D40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x70, D_00275670 + 0x2230);
    func_00102948(D_00816D40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x50, D_00275670 + 0xA0);

    func_00102948(D_00816E40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x60, D_00275670 + 0x2220);
    func_00102948(D_00816E40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x70, D_00275670 + 0x2230);
    func_00102948(D_00816E40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x50, D_00275670 + 0xA0);

    func_00102948(D_00816F40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x60, D_00275670 + 0x2220);
    func_00102948(D_00816F40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x70, D_00275670 + 0x2230);
    func_00102948(D_00816F40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x50, D_00275670 + 0xA0);

    func_00102948(D_00817040 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x60, D_00275670 + 0x2220);
    func_00102948(D_00817040 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x70, D_00275670 + 0x2230);
    func_00102948(D_00817040 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x50, D_00275670 + 0xA0);

    func_00102948(D_00817140 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x60, D_00275670 + 0x2220);
    func_00102948(D_00817140 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x70, D_00275670 + 0x2230);
    func_00102948(D_00817140 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x50, D_00275670 + 0xA0);

    func_00102948(D_00816940 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x60, D_00275670 + 0x2220);
    func_00102948(D_00816940 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x70, D_00275670 + 0x2230);
    func_00102948(D_00816940 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x50, D_00275670 + 0xA0);

    func_00102948(D_00816A40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x60, D_00275670 + 0x2220);
    func_00102948(D_00816A40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x70, D_00275670 + 0x2230);
    func_00102948(D_00816A40 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x50, D_00275670 + 0xA0);

    {
        char *ctx = D_00275670;

        *(int *)((char *)D_00816B98 + (*(int *)(ctx + 0x9c) << 7)) = 0x437F0000;
        *(int *)((char *)D_00816B9C + (*(int *)(ctx + 0x9c) << 7)) = 0;
        *(int *)((char *)D_00816C98 + (*(int *)(ctx + 0x9c) << 7)) = 0x437F0000;
        *(int *)((char *)D_00816C9C + (*(int *)(ctx + 0x9c) << 7)) = 0;

        func_00102948(D_00816540 + (*(int *)(ctx + 0x9c) << 7) + 0x60, ctx + 0x2220);
        func_00102948(D_00816540 + (*(int *)(ctx + 0x9c) << 7) + 0x70, ctx + 0x2230);
        func_00102948(D_00816540 + (*(int *)(ctx + 0x9c) << 7) + 0x50, ctx + 0xA0);

        *(int *)((char *)D_008168A0 + (*(int *)(ctx + 0x9c) << 7)) = 0x3A008081;
        *(int *)((char *)D_008168A4 + (*(int *)(ctx + 0x9c) << 7)) = 0x3A008081;
        *(int *)((char *)D_008168A8 + (*(int *)(ctx + 0x9c) << 7)) = 0x34008080;
        *(int *)((char *)D_008168AC + (*(int *)(ctx + 0x9c) << 7)) = 0;
        *(int *)((char *)D_008168B0 + (*(int *)(ctx + 0x9c) << 7)) = 0xBF808081;
        *(int *)((char *)D_008168B4 + (*(int *)(ctx + 0x9c) << 7)) = 0xBF808081;
        *(int *)((char *)D_008168B8 + (*(int *)(ctx + 0x9c) << 7)) = 0;
        *(int *)((char *)D_008168BC + (*(int *)(ctx + 0x9c) << 7)) = 0x3F800000;

        func_00102948(D_00816840 + (*(int *)(ctx + 0x9c) << 7) + 0x50, ctx + 0xA0);
    }

    {
        u128 *dst = D_002513E0;
        u128 *src = D_70003AC0;
        u128 tmp;
        tmp = *src;
        *dst = tmp;
        tmp = src[1];
        dst[1] = tmp;
        tmp = src[2];
        dst[2] = tmp;
        tmp = src[3];
        dst[3] = tmp;
    }
}
