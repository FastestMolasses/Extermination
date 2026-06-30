// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-frame weapon/aim setup for an actor (arg0). Builds an identity-ish
// transform in the scratch matrix block D_700036A0 (func_001029C0 init,
// func_00102BB0 rotate by the float at arg0+0xC4, func_00102918 with the
// position at arg0+0xB0), seeds the scratchpad params at 0x700038A0..AC
// (0, 10.0f, 5.5f, 1.0f), then func_001026A0 builds D_700038B0 from
// D_700036A0/D_700038A0. Fires func_0019AD00(arg0, D_700038B0, 0x80000007);
// on success, when the global *0x700031D0->byte[0x1A] == 0x32 and
// func_001782A0(arg0) is true, decrements the cooldown float at arg0+0x2E4
// by 6.0 and returns 1. Otherwise returns 0.
//
// mwcc 2.3.3 matches byte-identical; the pinned 991202 build caps at 91.0%
// (clean-store delay-slot scheduling: it reorders the lwc1 arg0+0xC4 load).
// Key: the two failure paths must share a single trailing `return 0` (nested
// ifs, NOT early `return 0` per arm) to reproduce the target's single B68
// return-0 block instead of duplicating the epilogue.
extern void func_001029C0(void *a0);
extern void func_00102BB0(void *a0, void *a1, float f);
extern void func_00102918(void *a0, void *a1, void *a2);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern int func_0019AD00(char *a0, void *a1, int a2);
extern int func_001782A0(char *a0);
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];

int func_00181A70(char *arg0) {
    func_001029C0(D_700036A0);
    func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg0 + 0xC4));
    func_00102918(D_700036A0, D_700036A0, arg0 + 0xB0);
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0x41200000;
    *(int *)0x700038A8 = 0x40B00000;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(D_700038B0, D_700036A0, D_700038A0);
    if (func_0019AD00(arg0, D_700038B0, 0x80000007) != 0) {
        if (*(unsigned char *)(*(char **)0x700031D0 + 0x1A) == 0x32) {
            if (func_001782A0(arg0) != 0) {
                *(float *)(arg0 + 0x2E4) = *(float *)(arg0 + 0x2E4) - 6.0f;
                return 1;
            }
        }
    }
    return 0;
}
