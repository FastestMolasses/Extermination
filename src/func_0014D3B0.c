// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Actor/enemy (re)spawn-init (sibling of func_0014C2E0/func_001516B0). Bumps the
// spawn-counter byte at arg0+4, sets active flag arg0+0 = 1, writes byte
// arg0+0xD = 0x80 and arg0+0x5D = 0x41, stores &D_00275410 (gp-relative small
// data) at arg0+0x30, seeds two 1.0f bit patterns on partner struct arg1
// (arg1+0x20, arg1+0x40), and caches D_0028A6FC at arg0+0x58. Difficulty branch
// on global flag D_0081070A: hard -> 0x34 timer halfword = 0x7D0, easy -> 0x4B0.
// Then func_001B10B0(arg0, 0x9D, 0x9C); if it returns 0, bone_init_default_2(arg0,0),
// arg0+0x20 = func_001EFE00(2, arg0), and byte arg0+5 = 4.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: 991202
// loses on the clean-store delay-slot / branch lowering (75.8%). D_00275410 must
// be small-data (sdatathreshold 8 -> gp_rel); D_0081070A and D_0028A6FC declared
// as arrays to force them OUT of small data so their loads stay %hi/%lo. Verified
// objdiff 100% vs build/expected/func_0014D3B0.o.
extern int func_001B10B0(char *arg0, int arg1, int arg2);
extern void bone_init_default_2(char *arg0, int arg1);
extern int func_001EFE00(int arg0, char *arg1);
extern int D_00275410;
extern int D_0028A6FC[4];
extern unsigned char D_0081070A[16];

void func_0014D3B0(char *arg0, char *arg1) {
    *(unsigned char *)(arg0 + 4) = *(unsigned char *)(arg0 + 4) + 1;
    *(char *)(arg0 + 0) = 1;
    *(unsigned char *)(arg0 + 0xD) = 0x80;
    *(int *)(arg1 + 0x20) = 0x3F800000;
    *(int *)(arg1 + 0x40) = 0x3F800000;
    *(int **)(arg0 + 0x30) = &D_00275410;
    *(unsigned char *)(arg0 + 0x5D) = 0x41;
    *(int *)(arg0 + 0x58) = D_0028A6FC[0];
    if (D_0081070A[0] != 0) {
        *(short *)(arg0 + 0x34) = 0x7D0;
    } else {
        *(short *)(arg0 + 0x34) = 0x4B0;
    }
    if (func_001B10B0(arg0, 0x9D, 0x9C) == 0) {
        bone_init_default_2(arg0, 0);
        *(int *)(arg0 + 0x20) = func_001EFE00(2, arg0);
        *(unsigned char *)(arg0 + 5) = 4;
    }
}
