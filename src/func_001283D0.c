// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Spawns/initializes an entity at the position in arg0 (xyz at +0,+4,+8) when
// the live-entity counter at *0x700031F4 is still below 0x15. Allocates via
// func_001AFA90(2); on success zeroes the byte fields at +3/+0x9A, copies the
// arg0 position into +0xB0..+0xB8, then branches on arg1: if arg1==1 the type
// byte +0xD = 0xC and a second xyz (from arg2) is stored at +0xC0..+0xC8; else
// type byte +0xD = 0xA and +0xC0..+0xC8 are zeroed. Installs the update handler
// func_0012A5D0 at +0x10, sets the four float fields +0x60..+0x6C (1.5/1.5/1.5/1.0),
// bumps the counter, and returns 1. Returns 0 if the cap is hit or alloc fails.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 96.9%. Keys: (1) the cap test written as `> 0x14` (not `>= 0x15`) reproduces the
// target's `slti at,v0,0x15` temp-register lowering; (2) the success path written
// inline as `if (p != 0) { ...; return 1; } return 0;` reproduces the `beqzl`
// branch-likely null check (delay slot = the v0=0 return clear).
extern char *func_001AFA90(int);
extern void func_0012A5D0();

int func_001283D0(float *arg0, int arg1, float *arg2) {
    char *p;
    int n;

    n = *(int *)0x700031F4;
    if (n > 0x14) {
        return 0;
    }
    p = func_001AFA90(2);
    if (p != 0) {
        *(char *)(p + 0x3) = 0;
        *(char *)(p + 0x9A) = 0;
        *(float *)(p + 0xB0) = arg0[0];
        *(float *)(p + 0xB4) = arg0[1];
        *(float *)(p + 0xB8) = arg0[2];
        if (arg1 == 1) {
            *(char *)(p + 0xD) = 0xC;
            *(float *)(p + 0xC0) = arg2[0];
            *(float *)(p + 0xC4) = arg2[1];
            *(float *)(p + 0xC8) = arg2[2];
        } else {
            *(char *)(p + 0xD) = 0xA;
            *(int *)(p + 0xC0) = 0;
            *(int *)(p + 0xC4) = 0;
            *(int *)(p + 0xC8) = 0;
        }
        *(int *)(p + 0x10) = (int)&func_0012A5D0;
        *(int *)(p + 0x60) = 0x3FC00000;
        *(int *)(p + 0x64) = 0x3FC00000;
        *(int *)(p + 0x68) = 0x3FC00000;
        *(int *)(p + 0x6C) = 0x3F800000;
        *(int *)0x700031F4 = *(int *)0x700031F4 + 1;
        return 1;
    }
    return 0;
}
