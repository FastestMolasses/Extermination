// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 92.08% NEARMISS).
// Fixes read from the original instructions:
//  - The context pointer D_00275670 is NOT held in a saved register across the
//    whole tail: it is read into a block-local only for each run of inline
//    stores plus the one copy call that follows; the other calls re-read the
//    global.
//  - The per-slot word stores index int arrays (`arr[frame * 32]`), not a
//    byte offset (`frame << 7`) added to a char pointer; that spelling decides
//    the store block's schedule.
//  - The final 4-quadword copy goes through two u128 pointers (src declared
//    first, dst assigned first) over char-array externs.
//
// Bulk per-slot-array initializer for the current frame slot
// (frame = *(int *)(D_00275670 + 0x9c), slot stride 0x80; sibling of
// skin_arena_init / func_001D38A0). Each of 12 base arrays gets three
// func_00102948 quadword copies at slot+0x60/+0x70/+0x50 from
// ctx+0x2220/+0x2230/+0xA0; slot words of D_00816B40/D_00816C40 at +0x58/+0x5C
// get 255.0f and 0; D_00816840 slot words +0x60..+0x7C get fixed float bit
// patterns before its +0x50 copy; finally the 64-byte scratchpad matrix
// D_70003AC0 is copied to D_002513E0.

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

extern int D_00816B98[];
extern int D_00816B9C[];
extern int D_00816C98[];
extern int D_00816C9C[];

extern int D_008168A0[];
extern int D_008168A4[];
extern int D_008168A8[];
extern int D_008168AC[];
extern int D_008168B0[];
extern int D_008168B4[];
extern int D_008168B8[];
extern int D_008168BC[];

typedef int u128 __attribute__((mode(TI)));

extern char D_002513E0[];
extern char D_70003AC0[];

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

        D_00816B98[*(int *)(ctx + 0x9c) * 32] = 0x437F0000;
        D_00816B9C[*(int *)(ctx + 0x9c) * 32] = 0;
        D_00816C98[*(int *)(ctx + 0x9c) * 32] = 0x437F0000;
        D_00816C9C[*(int *)(ctx + 0x9c) * 32] = 0;
        func_00102948(D_00816540 + (*(int *)(ctx + 0x9c) << 7) + 0x60, ctx + 0x2220);
    }
    func_00102948(D_00816540 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x70, D_00275670 + 0x2230);
    func_00102948(D_00816540 + (*(int *)(D_00275670 + 0x9c) << 7) + 0x50, D_00275670 + 0xA0);

    {
        char *ctx = D_00275670;

        D_008168A0[*(int *)(ctx + 0x9c) * 32] = 0x3A008081;
        D_008168A4[*(int *)(ctx + 0x9c) * 32] = 0x3A008081;
        D_008168A8[*(int *)(ctx + 0x9c) * 32] = 0x34008080;
        D_008168AC[*(int *)(ctx + 0x9c) * 32] = 0;
        D_008168B0[*(int *)(ctx + 0x9c) * 32] = 0xBF808081;
        D_008168B4[*(int *)(ctx + 0x9c) * 32] = 0xBF808081;
        D_008168B8[*(int *)(ctx + 0x9c) * 32] = 0;
        D_008168BC[*(int *)(ctx + 0x9c) * 32] = 0x3F800000;
        func_00102948(D_00816840 + (*(int *)(ctx + 0x9c) << 7) + 0x50, ctx + 0xA0);
    }

    {
        u128 *src;
        u128 *dst;
        dst = (u128 *)D_002513E0;
        src = (u128 *)D_70003AC0;
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
    }
}
