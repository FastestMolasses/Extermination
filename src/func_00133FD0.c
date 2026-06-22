// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Actor/entity init: bumps the per-instance counter at +0x4, sets state byte +0x0=1,
// flag byte +0xD=0x80, the paired control byte +0x5D=0x81, writes 1.0f to the linked
// record at arg1+0x20 and 0x1F to arg1+0x5B, installs the behavior/vtable pointer
// D_002753A0 at +0x30, copies the global word D_0028A704 to +0x58, and selects a
// timer at +0x34 (0x7D0 if the area flag D_0081070A is set, else 0x3E8). Then attempts
// func_001B10B0(self, 0x9C, 0x9B, counter); on failure (returns 0) it initializes the
// default bone set (bone_init_default_2) and forces the state byte +0x5 to 8.
//
// mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202 (78.98%). -sdatathreshold 8
// keeps D_002753A0 gp-relative while the [64] over-declarations push D_0028A704 /
// D_0081070A past the threshold so they stay %hi/%lo absolute. temp_a3 is a plain int
// with NO byte mask: the raw lbu+1 is passed to func_001B10B0 directly, which frees the
// beqz delay slot for the `paddub s0,a0` self-copy exactly as the target schedules it.
// Verified objdiff 100.0 vs build/expected/func_00133FD0.o.
extern void bone_init_default_2(char *, int);
extern int func_001B10B0(char *, int, int, int);
extern int D_002753A0;
extern int D_0028A704[64];
extern unsigned char D_0081070A[64];

void func_00133FD0(char *arg0, char *arg1) {
    int temp_a3;

    temp_a3 = *(unsigned char *)(arg0 + 4) + 1;
    *(unsigned char *)(arg0 + 4) = temp_a3;
    *(char *)(arg0 + 0) = 1;
    *(unsigned char *)(arg0 + 0xD) = 0x80;
    *(int *)(arg1 + 0x20) = 0x3F800000;
    *(char *)(arg1 + 0x5B) = 0x1F;
    *(int *)(arg0 + 0x30) = (int)&D_002753A0;
    *(unsigned char *)(arg0 + 0x5D) = 0x81;
    *(int *)(arg0 + 0x58) = D_0028A704[0];
    if (D_0081070A[0] != 0) {
        *(short *)(arg0 + 0x34) = 0x7D0;
    } else {
        *(short *)(arg0 + 0x34) = 0x3E8;
    }
    if (func_001B10B0(arg0, 0x9C, 0x9B, temp_a3) == 0) {
        bone_init_default_2(arg0, 0);
        *(char *)(arg0 + 5) = 8;
    }
}
