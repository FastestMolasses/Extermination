// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Actor/enemy (re)spawn-init (sibling of func_0014C2E0/func_00153A10). Bumps the
// spawn-counter byte at arg0+4, sets active flag arg0+0 = 1, writes bytes
// arg0+0xD = 0x80 and arg0+0x5D = 0x81, stores &D_00275430 (gp-relative small
// data) at arg0+0x30, seeds arg1+0x28 = 1.0f bit pattern, and caches D_0028A708
// at arg0+0x58. Difficulty branch on global D_0081070A: hard -> 0x34 timer
// halfword = 0xBB8, easy -> 0x7D0. Then func_001B10B0(arg0, 0xA0, 0x9F); if it
// returns 0, bone_init_default_2(arg0,0), arg0+0x20 = func_001EFE00(9, arg0),
// and byte arg0+5 = 9.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: 991202
// loses on the clean-store delay-slot / branch lowering (75.4%). D_00275430 must
// be small-data (sdatathreshold 8 -> gp_rel); D_0081070A and D_0028A708 declared
// as arrays to force them OUT of small data so their loads stay %hi/%lo. Verified
// objdiff 100% vs build/expected/func_001516B0.o.
extern int func_001B10B0(char *arg0, int arg1, int arg2);
extern void bone_init_default_2(char *arg0, int arg1);
extern int func_001EFE00(int arg0, char *arg1);
extern int D_00275430;
extern int D_0028A708[4];
extern unsigned char D_0081070A[16];

void func_001516B0(char *arg0, char *arg1) {
    *(unsigned char *)(arg0 + 4) = *(unsigned char *)(arg0 + 4) + 1;
    *(char *)(arg0 + 0) = 1;
    *(unsigned char *)(arg0 + 0xD) = 0x80;
    *(int **)(arg0 + 0x30) = &D_00275430;
    *(int *)(arg1 + 0x28) = 0x3F800000;
    *(unsigned char *)(arg0 + 0x5D) = 0x81;
    *(int *)(arg0 + 0x58) = D_0028A708[0];
    if (D_0081070A[0] != 0) {
        *(short *)(arg0 + 0x34) = 0xBB8;
    } else {
        *(short *)(arg0 + 0x34) = 0x7D0;
    }
    if (func_001B10B0(arg0, 0xA0, 0x9F) == 0) {
        bone_init_default_2(arg0, 0);
        *(int *)(arg0 + 0x20) = func_001EFE00(9, arg0);
        *(unsigned char *)(arg0 + 5) = 9;
    }
}
