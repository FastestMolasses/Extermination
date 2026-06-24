// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Actor/entity init. Bumps the per-instance counter at +0x4, sets state byte
// +0x0 = 1, writes the paired bytes arg1+0x8E = 1 and arg1+0x8F = (signed) the
// state-flag byte arg0+0xD, stores 1.0f at arg1+0x40, installs the gp-relative
// behavior pointer &D_002753B8 at arg0+0x30, and copies an 8-float matrix block
// from arg0+0xB0..0xCC into arg1+0x0..0x1C. Caches the global word D_0028A668
// at arg0+0x58. If the global byte D_00810808 == 0xFF it sets the 0x80 flag bit
// in arg0+0xD. Then branches on that 0x80 flag: the "hard" path (flag set) picks
// the 0x34 timer (0xC8 if area flag D_0081070A else 0x64), seeds the 0.5.../1.0
// quad at +0x60..0x6C and calls func_001B10B0(self, 0x77, 0x78); the other path
// picks timer 0x64/0x3C and calls func_001B10B0(self, 0x75, 0x78). On either
// call returning 0 it falls through to the shared tail bone_init_default_2(self,0).
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// only 84.76% (clean-store delay-slot / branch-lowering family). Keys: the two
// func_001B10B0 failure paths share one bone_init_default_2 common tail (early
// `return` in each arm); -sdatathreshold 8 keeps &D_002753B8 gp-relative while
// the [64] over-declarations push D_0028A668/D_00810808/D_0081070A out of small
// data so they stay %hi/%lo absolute; the 8-float copy is hand-unrolled; arg0+0xD
// is read as signed char and stored to a signed-char field at arg1+0x8F to emit
// `lb` (not `lbu`). Verified TRUE objdiff 100.0 byte-identical vs expected.
extern int func_001B10B0(char *arg0, int arg1, int arg2);
extern void bone_init_default_2(char *arg0, int arg1);
extern int D_002753B8;
extern int D_0028A668[64];
extern unsigned char D_00810808[64];
extern unsigned char D_0081070A[64];

void func_0013D490(char *arg0, char *arg1) {
    *(unsigned char *)(arg0 + 4) = *(unsigned char *)(arg0 + 4) + 1;
    *(char *)(arg0 + 0) = 1;
    *(unsigned char *)(arg1 + 0x8E) = 1;
    *(int *)(arg1 + 0x40) = 0x3F800000;
    *(char *)(arg1 + 0x8F) = *(char *)(arg0 + 0xD);
    *(int **)(arg0 + 0x30) = &D_002753B8;
    *(float *)(arg1 + 0x0) = *(float *)(arg0 + 0xB0);
    *(float *)(arg1 + 0x4) = *(float *)(arg0 + 0xB4);
    *(float *)(arg1 + 0x8) = *(float *)(arg0 + 0xB8);
    *(float *)(arg1 + 0xC) = *(float *)(arg0 + 0xBC);
    *(float *)(arg1 + 0x10) = *(float *)(arg0 + 0xC0);
    *(float *)(arg1 + 0x14) = *(float *)(arg0 + 0xC4);
    *(float *)(arg1 + 0x18) = *(float *)(arg0 + 0xC8);
    *(float *)(arg1 + 0x1C) = *(float *)(arg0 + 0xCC);
    *(int *)(arg0 + 0x58) = D_0028A668[0];
    if (D_00810808[0] == 0xFF) {
        *(unsigned char *)(arg0 + 0xD) = *(unsigned char *)(arg0 + 0xD) | 0x80;
    }
    if (*(unsigned char *)(arg0 + 0xD) & 0x80) {
        if (D_0081070A[0] != 0) {
            *(short *)(arg0 + 0x34) = 0xC8;
        } else {
            *(short *)(arg0 + 0x34) = 0x64;
        }
        *(int *)(arg0 + 0x60) = 0x3F8CCCCD;
        *(int *)(arg0 + 0x64) = 0x3F8CCCCD;
        *(int *)(arg0 + 0x68) = 0x3F8CCCCD;
        *(int *)(arg0 + 0x6C) = 0x3F800000;
        if (func_001B10B0(arg0, 0x77, 0x78) != 0) {
            return;
        }
    } else {
        if (D_0081070A[0] != 0) {
            *(short *)(arg0 + 0x34) = 0x64;
        } else {
            *(short *)(arg0 + 0x34) = 0x3C;
        }
        if (func_001B10B0(arg0, 0x75, 0x78) != 0) {
            return;
        }
    }
    bone_init_default_2(arg0, 0);
}
