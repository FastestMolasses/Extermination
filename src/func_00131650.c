// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Per-tick actor update: dispatches the state byte self+5 to its handler
// (0 func_00131740, 1 func_00131940, 2 func_00131B10, each (self, ctl)),
// releases a pending linked record self+0x20 (its byte +4 = 3, pointer
// cleared), runs func_001339E0 / func_00132490 / func_001328D0 (self, ctl),
// stores anim_advance_time(self, ctl+0x34 speed) into the halfword flags
// ctl+0x58, then func_00131ED0, func_001C68C0, func_001B17A0 on self and
// finally calls the per-actor hook at self+0x4C.

extern void func_00131740(char *arg0, char *arg1);
extern void func_00131940(char *arg0, char *arg1);
extern void func_00131B10(char *self, char *ctx);
extern void func_001339E0(char *a0, char *a1);
extern void func_00132490(char *p, char *e);
extern void func_001328D0(char *arg0, char *arg1);
extern int anim_advance_time(char *arg0, float dt);
extern void func_00131ED0(char *a0);
extern void func_001C68C0(char *o);
extern unsigned char func_001B17A0(char *self);

void func_00131650(char *self, char *ctl) {
    char *pending;

    switch (*(unsigned char *)(self + 5)) {
    case 0:
        func_00131740(self, ctl);
        break;
    case 1:
        func_00131940(self, ctl);
        break;
    case 2:
        func_00131B10(self, ctl);
        break;
    }
    pending = *(char **)(self + 0x20);
    if (pending != 0) {
        *(unsigned char *)(pending + 4) = 3;
        *(char **)(self + 0x20) = 0;
    }
    func_001339E0(self, ctl);
    func_00132490(self, ctl);
    func_001328D0(self, ctl);
    *(short *)(ctl + 0x58) = anim_advance_time(self, *(float *)(ctl + 0x34));
    func_00131ED0(self);
    func_001C68C0(self);
    func_001B17A0(self);
    (*(void (**)(char *))(self + 0x4C))(self);
}
