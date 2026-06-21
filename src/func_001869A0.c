// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Missile-launcher fire — sub-weapon 1 fire-event arm of the weapon-actor
// dispatcher func_00188630. Spawns a class-1 actor (func_001AFA90(1)): model
// byte +0x03 = 3, behavior +0x10 = func_0018AF50 (guided projectile:
// per-frame steer + segment queries), position +0xB0..B8 and direction
// +0xC0..C8 copied from the gun actor; +0x24 = locked target (D_008106E0)
// when the player context D_008104E0 == 0xC and aim option D_00810CA4 == 1,
// else NULL (unguided). Returns 0.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the lone
// residual under 991202 was wall #13 (it fills the `beqz v0` delay slot with
// the safe `li v1,3` where CW leaves a nop). The 2.3.3 build leaves that nop,
// so this readable C is byte-identical. Verified objdiff 100% vs
// build/expected/func_001869A0.o (991202 = 93.3%).
extern char *func_001AFA90(int);
extern int func_0018AF50;
extern int D_008104E0;
extern unsigned char D_00810CA4;
extern int D_008106E0;

int func_001869A0(char *arg0) {
    char *v0;

    v0 = func_001AFA90(1);
    if (v0 != 0) {
        *(char *)(v0 + 3) = 3;
        *(int *)(v0 + 0x10) = (int)&func_0018AF50;
        *(float *)(v0 + 0xB0) = *(float *)(arg0 + 0xB0);
        *(float *)(v0 + 0xB4) = *(float *)(arg0 + 0xB4);
        *(float *)(v0 + 0xB8) = *(float *)(arg0 + 0xB8);
        *(float *)(v0 + 0xC0) = *(float *)(arg0 + 0xC0);
        *(float *)(v0 + 0xC4) = *(float *)(arg0 + 0xC4);
        *(float *)(v0 + 0xC8) = *(float *)(arg0 + 0xC8);
        if (D_008104E0 == 0xC && D_00810CA4 == 1) {
            *(int *)(v0 + 0x24) = D_008106E0;
        } else {
            *(int *)(v0 + 0x24) = 0;
        }
    }
    return 0;
}
