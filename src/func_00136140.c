// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Per-tick actor update: dispatches the state byte self+5 (0 func_00136240,
// 1 func_00136490, each (self, ctl)). When the low byte of the anim word ctl+0
// is 9 it fires a random sound/event 0x841 + (rand >> 16) % 10 via
// func_001FBD50(self, id, 0, 300.0). Then func_001B5360(self),
// func_00136630 / func_001366E0 (self, ctl), stores anim_advance_time(self,
// 1.0) into ctl+0, runs func_00131ED0, func_001C68C0, func_001B17A0 and the
// per-actor hook at self+0x4C.

extern void func_00136240(char *arg0, char *arg1);
extern void func_00136490(char *p, char *a1);
extern int func_00122BB8(void);
extern void func_001FBD50(char *p, int a, int b, float f);
extern void func_001B5360(char *p);
extern void func_00136630(char *a0, char *a1);
extern void func_001366E0(char *self, char *owner);
extern int anim_advance_time(char *arg0, float dt);
extern void func_00131ED0(char *a0);
extern void func_001C68C0(char *o);
extern unsigned char func_001B17A0(char *self);

void func_00136140(char *self, char *ctl) {
    switch (*(unsigned char *)(self + 5)) {
    case 0:
        func_00136240(self, ctl);
        break;
    case 1:
        func_00136490(self, ctl);
        break;
    }
    if (*(unsigned char *)ctl == 9) {
        func_001FBD50(self, 0x841 + (func_00122BB8() >> 16) % 10, 0, 300.0f);
    }
    func_001B5360(self);
    func_00136630(self, ctl);
    func_001366E0(self, ctl);
    *(int *)ctl = anim_advance_time(self, 1.0f);
    func_00131ED0(self);
    func_001C68C0(self);
    func_001B17A0(self);
    (*(void (**)(char *))(self + 0x4C))(self);
}
