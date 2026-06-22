// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Actor/enemy (re)spawn-init. Bumps the spawn-counter byte at arg0+4, sets the
// active flag (arg0+0 = 1), seeds two floats on the partner struct arg1
// (arg1+0x20 = 1.0f bit pattern, arg1+0x18 = int 0xF0), and stores &D_00275408
// (gp-relative small-data global) at arg0+0x30. Difficulty branch on the global
// flag D_0081070A: hard -> 0x34 timer halfword = 0xA, easy -> 0x5. Writes a 0.5/
// 0.5/0.5/1.0 float-bit quad to arg0+0x60..0x6C and caches D_0028A720 at arg0+0x58.
// Then func_001B10B0(arg0, 0xA6, 0xA5); if it returns 0, bone_init_default_2(arg0,0)
// and copies byte arg0+0xD into arg0+5.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: 991202
// loses on the clean-store delay-slot / D_0081070A branch lowering (84.0%).
// D_00275408 must be small-data (sdatathreshold 8 -> gp_rel); D_0081070A and
// D_0028A720 are declared as arrays to force them OUT of small data so their
// loads stay %hi/%lo absolute, matching the target. Verified objdiff 100% vs
// build/expected/func_0014C2E0.o.
extern int func_001B10B0(char *arg0, int arg1, int arg2);
extern void bone_init_default_2(char *arg0, int arg1);
extern int D_00275408;
extern int D_0028A720[4];
extern unsigned char D_0081070A[16];

void func_0014C2E0(char *arg0, char *arg1) {
    *(unsigned char *)(arg0 + 4) = *(unsigned char *)(arg0 + 4) + 1;
    *(char *)(arg0 + 0) = 1;
    *(int *)(arg1 + 0x20) = 0x3F800000;
    *(int *)(arg1 + 0x18) = 0xF0;
    *(int **)(arg0 + 0x30) = &D_00275408;
    if (D_0081070A[0] != 0) {
        *(short *)(arg0 + 0x34) = 0xA;
    } else {
        *(short *)(arg0 + 0x34) = 0x5;
    }
    *(int *)(arg0 + 0x60) = 0x3F000000;
    *(int *)(arg0 + 0x64) = 0x3F000000;
    *(int *)(arg0 + 0x68) = 0x3F000000;
    *(int *)(arg0 + 0x6C) = 0x3F800000;
    *(int *)(arg0 + 0x58) = D_0028A720[0];
    if (func_001B10B0(arg0, 0xA6, 0xA5) == 0) {
        bone_init_default_2(arg0, 0);
        *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 0xD);
    }
}
