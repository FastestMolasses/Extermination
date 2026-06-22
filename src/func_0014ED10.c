// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Actor/enemy (re)spawn-init (sibling of func_0014D3B0/func_0014C2E0). Bumps the
// spawn-counter byte at arg0+4, sets active flag arg0+0 = 1 and byte arg0+0x5D =
// 0x81, stores &D_00275420 (gp-relative small data) at arg0+0x30, seeds a 1.0f
// bit pattern on partner struct arg1+0x20, and caches D_0028A6FC at arg0+0x58.
// Difficulty branch on global flag D_0081070A: hard -> 0x34 timer halfword = 0x5DC,
// easy -> 0x3E8. Then func_001B10B0(arg0, 0x9D, 0x9C); if it returns 0,
// bone_init_default_2(arg0,0), arg0+0x20 = func_001EFE00(6, arg0), zeroes the
// arg0+0xC0/0xC4/0xC8 int triple and writes 1.0f bits to arg0+0xCC.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: 991202
// loses on the clean-store delay-slot / branch lowering (80.2%). D_00275420 must
// be small-data (sdatathreshold 8 -> gp_rel); D_0081070A and D_0028A6FC declared
// as arrays to force them OUT of small data so their loads stay %hi/%lo. Verified
// objdiff 100% vs build/expected/func_0014ED10.o.
extern int func_001B10B0(char *arg0, int arg1, int arg2);
extern void bone_init_default_2(char *arg0, int arg1);
extern int func_001EFE00(int arg0, char *arg1);
extern int D_00275420;
extern int D_0028A6FC[4];
extern unsigned char D_0081070A[16];

void func_0014ED10(char *arg0, char *arg1) {
    *(unsigned char *)(arg0 + 4) = *(unsigned char *)(arg0 + 4) + 1;
    *(char *)(arg0 + 0) = 1;
    *(int *)(arg1 + 0x20) = 0x3F800000;
    *(int **)(arg0 + 0x30) = &D_00275420;
    *(unsigned char *)(arg0 + 0x5D) = 0x81;
    *(int *)(arg0 + 0x58) = D_0028A6FC[0];
    if (D_0081070A[0] != 0) {
        *(short *)(arg0 + 0x34) = 0x5DC;
    } else {
        *(short *)(arg0 + 0x34) = 0x3E8;
    }
    if (func_001B10B0(arg0, 0x9D, 0x9C) == 0) {
        bone_init_default_2(arg0, 0);
        *(int *)(arg0 + 0x20) = func_001EFE00(6, arg0);
        *(int *)(arg0 + 0xC8) = 0;
        *(int *)(arg0 + 0xC4) = 0;
        *(int *)(arg0 + 0xC0) = 0;
        *(int *)(arg0 + 0xCC) = 0x3F800000;
    }
}
