// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Grenade-launcher fire — sub-weapon 4 fire-event arm of the weapon-actor
// dispatcher func_00188630. Spawns a class-1 actor (func_001AFA90(1)):
// model byte +0x03 = 3, behavior +0x10 = func_0018B3E0 (ballistic grenade
// projectile), param +0x0D = D_0081070B (area sub-byte), position +0xB0..B8
// from the gun actor's launch point (+0xB0), initial velocity +0x70..78 from
// the gun's aim direction vector (+0xC0). Returns 0.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the lone
// residual under 991202 was wall #13 (it fills the `beqz v0` delay slot with
// the safe `li v1,3` where CW leaves a nop). The 2.3.3 build leaves that nop,
// so this readable C is byte-identical. Verified objdiff 100% vs
// build/expected/func_001872C0.o.
extern char *func_001AFA90(int);
extern unsigned char D_0081070B;
extern int func_0018B3E0;

int func_001872C0(char *arg0) {
    char *v0;

    v0 = func_001AFA90(1);
    if (v0 != 0) {
        *(char *)(v0 + 3) = 3;
        *(int *)(v0 + 0x10) = (int)&func_0018B3E0;
        *(unsigned char *)(v0 + 0xD) = D_0081070B;
        *(float *)(v0 + 0xB0) = *(float *)(arg0 + 0xB0);
        *(float *)(v0 + 0xB4) = *(float *)(arg0 + 0xB4);
        *(float *)(v0 + 0xB8) = *(float *)(arg0 + 0xB8);
        *(float *)(v0 + 0x70) = *(float *)(arg0 + 0xC0);
        *(float *)(v0 + 0x74) = *(float *)(arg0 + 0xC4);
        *(float *)(v0 + 0x78) = *(float *)(arg0 + 0xC8);
    }
    return 0;
}
