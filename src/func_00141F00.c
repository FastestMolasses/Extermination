// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Actor/entity spawn-state initialiser. Increments the spawn counter byte at
// +4, sets state byte +0 = 1, writes 1.0f into the source struct (arg1) at
// +0x40, stamps the address of the gp-rel constant D_002753D0 into record+0x30,
// then runs func_00102948 twice (arg1<->record+0xB0 channel setup, with the
// spawn counter as the 4th arg on the first call). Copies global D_0028A690 to
// record+0x58. If the global flag D_00810808 == 0xFF, OR's 0x80 into the status
// byte at +0xD. Then branches on that 0x80 bit:
//   set   -> timer +0x34 = 500 or 300 (per D_0081070A), three floats at
//            +0x60..+0x68 = D_002753D8, +0x6C = 1.0f, then func_001B10B0(rec,
//            0x7E,0x7F); if it returns 0, run bone_init_default_2(rec,0).
//   clear -> timer +0x34 = 350 or 200 (per D_0081070A), +0x60..+0x68 = 1.5f,
//            +0x6C = 1.0f, then func_001B10B0(rec,0x7D,0x7F); if 0, bone_init.
// Both arms share one bone_init_default_2 tail (the original cross-jumps the
// two identical calls into a single block), expressed here with `goto`.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): the pinned 991202 build caps at
// 78.1% (it fills the func_001B10B0 clean-store delay slot differently and does
// not perform the shared-tail cross-jump the same way). D_002753D0/D_002753D8
// are gp-rel (sdatathreshold 8 -> %gp_rel); D_0028A690/D_00810808/D_0081070A are
// sized [16] so they stay hi/lo. The 0x12C/0x1F4/0xC8/0x15E timer values are
// stored via per-branch `sh` (tail-duplicated). Verified objdiff 100% vs
// build/expected/func_00141F00.o.
extern void func_00102948();
extern int func_001B10B0(char *a, int b, int c);
extern void bone_init_default_2(char *a, int b);
extern int D_002753D0;
extern float D_002753D8;
extern int D_0028A690[16];
extern unsigned char D_0081070A[16];
extern unsigned char D_00810808[16];

void func_00141F00(char *arg0, char *arg1) {
    int a3;

    a3 = *(unsigned char *)(arg0 + 4) + 1;
    *(unsigned char *)(arg0 + 4) = a3;
    *(unsigned char *)(arg0 + 0) = 1;
    *(float *)(arg1 + 0x40) = 1.0f;
    *(int *)(arg0 + 0x30) = (int)&D_002753D0;
    func_00102948(arg1, arg0 + 0xB0, 1, a3);
    func_00102948(arg1 + 0x10, arg0 + 0xB0);
    *(int *)(arg0 + 0x58) = D_0028A690[0];
    if (D_00810808[0] == 0xFF) {
        *(unsigned char *)(arg0 + 0xD) = *(unsigned char *)(arg0 + 0xD) | 0x80;
    }
    if (*(unsigned char *)(arg0 + 0xD) & 0x80) {
        if (D_0081070A[0] != 0) {
            *(short *)(arg0 + 0x34) = (short)0x1F4;
        } else {
            *(short *)(arg0 + 0x34) = (short)0x12C;
        }
        *(float *)(arg0 + 0x60) = D_002753D8;
        *(float *)(arg0 + 0x64) = D_002753D8;
        *(float *)(arg0 + 0x68) = D_002753D8;
        *(int *)(arg0 + 0x6C) = 0x3F800000;
        if (func_001B10B0(arg0, 0x7E, 0x7F) == 0) {
            goto shared_bone;
        }
        return;
    } else {
        if (D_0081070A[0] != 0) {
            *(short *)(arg0 + 0x34) = (short)0x15E;
        } else {
            *(short *)(arg0 + 0x34) = (short)0xC8;
        }
        *(int *)(arg0 + 0x60) = 0x3FC00000;
        *(int *)(arg0 + 0x64) = 0x3FC00000;
        *(int *)(arg0 + 0x68) = 0x3FC00000;
        *(int *)(arg0 + 0x6C) = 0x3F800000;
        if (func_001B10B0(arg0, 0x7D, 0x7F) != 0) {
            return;
        }
    }
shared_bone:
    bone_init_default_2(arg0, 0);
}
