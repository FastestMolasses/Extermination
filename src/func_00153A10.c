// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Actor (re)spawn/init: bumps a counter byte at arg0+4, sets active flags
// (arg0+0 = 1, arg1+0x10 = 1), sets a 0x3C timer halfword at arg0+0x34, and
// stores &D_00275440 (a small-data global, gp-relative) at arg0+0x30.
// Then calls func_001B10B0(arg0, 0x72, 0x71); if it returns 0, runs
// bone_init_default_2(arg0, 0x33) and caches D_0028A65C into arg0+0x58.
// D_0028A65C is declared as an array so it stays out of small-data (matches
// the %hi/%lo addressing) while D_00275440 stays gp-relative.
extern int func_001B10B0(char *arg0, int arg1, int arg2);
extern void bone_init_default_2(char *arg0, int arg1);
extern int D_00275440;
extern int D_0028A65C[4];

void func_00153A10(char *arg0, char *arg1) {
    *(unsigned char *)(arg0 + 4) = *(unsigned char *)(arg0 + 4) + 1;
    *(char *)(arg0 + 0) = 1;
    *(char *)(arg1 + 0x10) = 1;
    *(short *)(arg0 + 0x34) = 0x3C;
    *(int **)(arg0 + 0x30) = &D_00275440;
    if (func_001B10B0(arg0, 0x72, 0x71) == 0) {
        bone_init_default_2(arg0, 0x33);
        *(int *)(arg0 + 0x58) = D_0028A65C[0];
    }
}
