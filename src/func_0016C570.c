// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Sets up two symmetric particle/effect emitters around the actor (arg0).
// func_001029C0 inits the shared work block D_700036A0; func_00102BB0 feeds it
// the actor float at +0x218; func_00102918 binds the actor's +0xB0 vector. Then
// two passes fill the param block at 0x700038A0 (vec4 = {+/-4.5f, 0.1f, 0.0f, 1.0f}),
// build it via func_001026A0, and dispatch func_0019AD00(actor, block, 0x80000006).
extern int func_001029C0();
extern int func_00102BB0(void *, void *, float);
extern int func_00102918();
extern int func_001026A0();
extern int func_0019AD00();
extern int D_700036A0;
extern int D_700038A0;
extern int D_700038B0;

void func_0016C570(char *arg0) {
    func_001029C0(&D_700036A0);
    func_00102BB0(&D_700036A0, &D_700036A0, *(float *)(arg0 + 0x218));
    func_00102918(&D_700036A0, &D_700036A0, arg0 + 0xB0);
    *(int *)0x700038A0 = 0x40900000;
    *(int *)0x700038A4 = 0x3DCCCCCD;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(&D_700038B0, &D_700036A0, &D_700038A0);
    func_0019AD00(arg0, &D_700038B0, 0x80000006);
    *(int *)0x700038A0 = 0xC0900000;
    *(int *)0x700038A4 = 0x3DCCCCCD;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(&D_700038B0, &D_700036A0, &D_700038A0);
    func_0019AD00(arg0, &D_700038B0, 0x80000006);
}
