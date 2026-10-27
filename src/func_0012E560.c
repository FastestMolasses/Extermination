// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 91.53% NEARMISS).
// Fixes read from the original instructions:
// - Wrong body: both flag-bit-0 gate paths (clips 0x70 and 0x6E) store
//   D_0028A64C into p[0x58]; both bit-0-clear paths (0x74 and 0x72) store
//   D_0028A65C. The old goto shape sent the 0x70 path to D_0028A65C.
// - func_00102948 takes two arguments (dst, src), as its other callers show.
// - D_00810808, D_0081070A, D_0028A64C and D_0028A65C are extern arrays
//   (absolute addressing under -sdatathreshold 8, as in func_00138540).
// - The q[0x61] byte clear and the q[0x58] halfword clear are one chained
//   assignment (the halfword gets the byte-typed zero).
//
// Spawn/init for an owner record: bumps the spawn counter p[4], marks p
// active, initializes the paired record q (flags copy from p[0xD], scale
// 1.0f), installs the gp-base default table &D_00275388 at p[0x30], copies two
// matrices via func_00102948, ORs bit 0x80 into p[0xD] when D_00810808 is 0xFF,
// then runs the func_001B10B0 state gate (returns early if it reports busy),
// picks the p[0x58] base by flag bit 0, and sets the p[0x34] timer from a
// switch on (p[0xD] & 0x83) with D_0081070A selecting each pair (the 0x80/0x81
// cases also seed the 0x60..0x6C scale quad). Finally kicks anim clip 5 when
// p[0x56] is non-zero.

extern void func_00102948(char *dst, char *src);
extern int func_001B10B0(char *a, int b, int c);
extern void bone_init_default_2(char *a, int b);
extern void anim_clip_init(char *self, int clip, float a, float b);
extern int D_00275388;
extern int D_0028A64C[];
extern int D_0028A65C[];
extern unsigned char D_0081070A[];
extern unsigned char D_00810808[];

void func_0012E560(char *arg0, char *arg1) {
    int a2;

    a2 = *(unsigned char *)(arg0 + 4) + 1;
    *(unsigned char *)(arg0 + 4) = a2;
    *(unsigned char *)(arg0 + 0) = 1;
    *(unsigned char *)(arg1 + 0x6D) = 1;
    *(unsigned char *)(arg1 + 0x6B) = 0;
    *(int *)(arg1 + 0x34) = 0x3F800000;
    a2 = *(unsigned char *)(arg0 + 0xD);
    *(unsigned char *)(arg1 + 0x6E) = a2;
    *(short *)(arg1 + 0x58) = *(unsigned char *)(arg1 + 0x61) = 0;
    *(int *)(arg0 + 0x30) = (int)&D_00275388;
    *(int *)(arg0 + 0x20) = 0;
    func_00102948(arg1, arg0 + 0xB0);
    func_00102948(arg1 + 0x20, arg0 + 0xC0);
    if (D_00810808[0] == 0xFF) {
        *(unsigned char *)(arg0 + 0xD) = *(unsigned char *)(arg0 + 0xD) | 0x80;
    }
    if (*(unsigned char *)(arg0 + 0xD) & 1) {
        if (*(unsigned char *)(arg0 + 0xD) & 0x80) {
            if (func_001B10B0(arg0, 0x70, 0x71) != 0) {
                return;
            }
        } else {
            if (func_001B10B0(arg0, 0x6E, 0x71) != 0) {
                return;
            }
        }
        *(int *)(arg0 + 0x58) = D_0028A64C[0];
    } else {
        if (*(unsigned char *)(arg0 + 0xD) & 0x80) {
            if (func_001B10B0(arg0, 0x74, 0x71) != 0) {
                return;
            }
        } else {
            if (func_001B10B0(arg0, 0x72, 0x71) != 0) {
                return;
            }
        }
        *(int *)(arg0 + 0x58) = D_0028A65C[0];
    }
    bone_init_default_2(arg0, 5);
    switch (*(unsigned char *)(arg0 + 0xD) & 0x83) {
    case 0:
        if (D_0081070A[0] != 0) {
            *(short *)(arg0 + 0x34) = 0x78;
        } else {
            *(short *)(arg0 + 0x34) = 0x50;
        }
        break;
    case 0x80:
        if (D_0081070A[0] != 0) {
            *(short *)(arg0 + 0x34) = 0xFA;
        } else {
            *(short *)(arg0 + 0x34) = 0x96;
        }
        *(int *)(arg0 + 0x60) = 0x3FA66666;
        *(int *)(arg0 + 0x64) = 0x3FA66666;
        *(int *)(arg0 + 0x68) = 0x3FA66666;
        *(int *)(arg0 + 0x6C) = 0x3F800000;
        break;
    case 1:
        if (D_0081070A[0] != 0) {
            *(short *)(arg0 + 0x34) = 0x96;
        } else {
            *(short *)(arg0 + 0x34) = 0x64;
        }
        break;
    case 0x81:
        if (D_0081070A[0] != 0) {
            *(short *)(arg0 + 0x34) = 0x12C;
        } else {
            *(short *)(arg0 + 0x34) = 0xB4;
        }
        *(int *)(arg0 + 0x60) = 0x3FA66666;
        *(int *)(arg0 + 0x64) = 0x3FA66666;
        *(int *)(arg0 + 0x68) = 0x3FA66666;
        *(int *)(arg0 + 0x6C) = 0x3F800000;
        break;
    case 3:
        if (D_0081070A[0] != 0) {
            *(short *)(arg0 + 0x34) = 0x15E;
        } else {
            *(short *)(arg0 + 0x34) = 0xC8;
        }
        break;
    }
    if (*(short *)(arg0 + 0x56) != 0) {
        *(unsigned char *)(arg0 + 5) = 0;
        *(unsigned char *)(arg0 + 6) = 7;
        anim_clip_init(arg0, 5, 10.0f, 0.0f);
    }
}
