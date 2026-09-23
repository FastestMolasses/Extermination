// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// MATCH NOTE (m2-matching lane, 88.64% -> 100.0%): the two masked halfword
// updates are 64-bit BITFIELD stores. The target keeps the unfolded
// mask of zero with 0x1FF and a 64-bit add forming 0x38, which is what mwcc
// emits for `field = 0` / `field = 0x38` into an `unsigned long long x : 9`
// member; an explicit int mask expression folds both. The layout is the GS
// DISPFB register (FBP:9 FBW:6 PSM:5 -:12 DBX:11 DBY:11). The globals are
// `volatile` so the second load is not hoisted above the first store, which
// is the order the target keeps.
//
// Semantics: sets up the two display environments of the double buffer.
// func_001002E0 (disp, psm, width, height, dx, dy) is called with psm 0,
// 512 x 224, dx = arg0 and dy = arg1 * 2 for D_00810EA0 and D_00810EC8
// (0x28 bytes apart). Then each environment's DISPFB (+0x10) frame-buffer
// base FBP is set: 0 for the first and 0x38 for the second.
typedef struct {
    unsigned long long FBP : 9;
    unsigned long long FBW : 6;
    unsigned long long PSM : 5;
    unsigned long long pad0 : 12;
    unsigned long long DBX : 11;
    unsigned long long DBY : 11;
    unsigned long long pad1 : 10;
} GsDispfb;

extern void func_001002E0(void *, int, int, int, int, short);
extern unsigned char D_00810EA0;
extern volatile GsDispfb D_00810EB0;
extern unsigned char D_00810EC8;
extern volatile GsDispfb D_00810ED8;

void func_001AB4E0(int arg0, int arg1) {
    short s = (short)(arg1 << 1);
    func_001002E0(&D_00810EA0, 0, 0x200, 0xE0, arg0, s);
    func_001002E0(&D_00810EC8, 0, 0x200, 0xE0, arg0, s);
    D_00810EB0.FBP = 0;
    D_00810ED8.FBP = 0x38;
}
