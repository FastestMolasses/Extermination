// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Per-tick actor update: dispatches the state byte self+5 (0 func_0013B420,
// 1 func_0013B5B0, 2 func_0013B790, each (self, ctl)), runs func_0013BF20 and
// func_0013BE60 (self, ctl), stores anim_advance_time(self, 1.0) into ctl+0x70,
// then func_00131ED0, func_001C6910, func_001B17A0 on self and the per-actor
// hook at self+0x4C.

extern void func_0013B420(char *arg0, char *arg1);
extern void func_0013B5B0(char *arg0, char *arg1);
extern void func_0013B790(char *arg0, char *arg1);
extern void func_0013BF20(char *arg0, char *arg1);
extern void func_0013BE60(char *arg0, char *arg1);
extern int anim_advance_time(char *arg0, float dt);
extern void func_00131ED0(char *a0);
extern void func_001C6910(char *o);
extern unsigned char func_001B17A0(char *self);

void func_0013B350(char *self, char *ctl) {
    switch (*(unsigned char *)(self + 5)) {
    case 0:
        func_0013B420(self, ctl);
        break;
    case 1:
        func_0013B5B0(self, ctl);
        break;
    case 2:
        func_0013B790(self, ctl);
        break;
    }
    func_0013BF20(self, ctl);
    func_0013BE60(self, ctl);
    *(int *)(ctl + 0x70) = anim_advance_time(self, 1.0f);
    func_00131ED0(self);
    func_001C6910(self);
    func_001B17A0(self);
    (*(void (**)(char *))(self + 0x4C))(self);
}
