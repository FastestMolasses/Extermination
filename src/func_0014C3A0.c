// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Mode-1 tick of the func_0014C220 sub-actor, (e, st). Runs the behaviour
// handler for the state byte e[5] (0 func_0014C680, 1 func_0014C9D0,
// 2 func_0014C490; other values none), then sets the heading e+0xC0 =
// func_001B1470(-func_0011E620(st+0x24, st+0x2C)) (func_0011E620 is an
// atan2-style wrapper; the angle is negated before func_001B1470), and runs func_001B5360, func_0014CD30,
// func_0014CDD0, st+0x00 = anim_advance_time(e, st+0x20), func_001C68C0,
// func_001B17A0 and the unconditional hook at e+0x4C.
extern void func_0014C680(char *e, char *st);
extern void func_0014C9D0(char *e, char *st);
extern void func_0014C490(char *e, char *st);
extern float func_0011E620(float y, float x);
extern float func_001B1470(float angle);
extern void func_001B5360(char *e);
extern void func_0014CD30(char *e, char *st);
extern void func_0014CDD0(char *e, char *st);
extern int anim_advance_time(char *e, float t);
extern void func_001C68C0(char *e);
extern int func_001B17A0(char *e);

void func_0014C3A0(char *e, char *st) {
    switch (*(unsigned char *)(e + 5)) {
    case 0:
        func_0014C680(e, st);
        break;
    case 1:
        func_0014C9D0(e, st);
        break;
    case 2:
        func_0014C490(e, st);
        break;
    }
    *(float *)(e + 0xC0) = func_001B1470(-1.0f * func_0011E620(*(float *)(st + 0x24), *(float *)(st + 0x2C)));
    func_001B5360(e);
    func_0014CD30(e, st);
    func_0014CDD0(e, st);
    *(int *)st = anim_advance_time(e, *(float *)(st + 0x20));
    func_001C68C0(e);
    func_001B17A0(e);
    (*(void (**)(char *))(e + 0x4C))(e);
}
