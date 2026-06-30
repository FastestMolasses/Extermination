// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Player/camera aim-tracking step. Builds a transform via the matrix helpers
// at &D_700036A0 (func_001029C0 init, func_00102C58 with the actor's +0xC0
// aim vector, func_00102918 with the +0xB0 position), seeds the projection
// constants at 0x700038A0..AC (0, 18.0f, 5.0f, 1.0f) and runs func_001026A0
// into &D_700038B0. If func_0019AD00(actor, &D_700038B0, 6) succeeds it reads
// the active camera (*0x700031D0), computes a target yaw = atan2(-cam.z(+0x2C),
// cam.x(+0x24)) (func_0011E620) stored at 0x70003A20, wraps (4.712389f + yaw)
// and the delta to the actor's current yaw (+0xC4) through func_001B1470, and
// if |delta| (func_0011DF78) <= 0.43982297f advances the actor's tracked
// position (+0xB0/+0xB8 = cam base 0x700031B0/B8 + 1.5 * cam.x/cam.z) and
// commits the new yaw to +0xC4. $f20 (saved across the 0x8 frame gap) carries
// the wrapped yaw to the commit.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): the pinned 991202 build walls at
// 94% on the idiom-13 clean-store delay-slot nop; 2.3.3 leaves the nop, giving
// a byte-identical .text. Verified objdiff 100% (cmp of .text) vs
// build/expected/func_00182250.o.
extern void func_001029C0(void *);
extern void func_00102C58(void *, void *, void *);
extern void func_00102918(void *, void *, void *);
extern void func_001026A0(void *, void *, void *);
extern int func_0019AD00(char *, void *, int);
extern float func_0011E620(float, float);
extern float func_001B1470(float);
extern float func_0011DF78(float);
extern int D_700036A0;
extern int D_700038A0;
extern int D_700038B0;

void func_00182250(char *arg0) {
    float f20;
    float t;
    char *p;

    func_001029C0(&D_700036A0);
    func_00102C58(&D_700036A0, &D_700036A0, arg0 + 0xC0);
    func_00102918(&D_700036A0, &D_700036A0, arg0 + 0xB0);
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0x41A00000;
    *(int *)0x700038A8 = 0x40A00000;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(&D_700038B0, &D_700036A0, &D_700038A0);
    if (func_0019AD00(arg0, &D_700038B0, 6) != 0) {
        p = *(char **)0x700031D0;
        *(float *)0x70003A20 = func_0011E620(-*(float *)(p + 0x2C), *(float *)(p + 0x24));
        f20 = func_001B1470(4.712389f + *(float *)0x70003A20);
        t = func_001B1470(f20 - *(float *)(arg0 + 0xC4));
        *(float *)0x70003A24 = t;
        if (func_0011DF78(t) <= 0.43982297f) {
            *(float *)(arg0 + 0xB0) = *(float *)0x700031B0 + 1.5f * *(float *)(*(char **)0x700031D0 + 0x24);
            *(float *)(arg0 + 0xB8) = *(float *)0x700031B8 + 1.5f * *(float *)(*(char **)0x700031D0 + 0x2C);
            *(float *)(arg0 + 0xC4) = f20;
        }
    }
}
