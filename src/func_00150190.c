// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Per-tick driver for this actor: dispatches the state byte self+5 to its
// handler (0: func_00150260 then func_001513B0, 1: func_001504D0, 2:
// func_00150600; each takes (self, ctl)), then runs func_00150900(self, ctl),
// stores anim_advance_time(self, ctl+0x20 speed) into the word ctl+0, and
// calls func_00150850, func_001C68C0 and func_001B17A0 on self before the
// per-actor hook at self+0x4C.
extern void func_00150260(char *arg0, char *arg1);
extern void func_001504D0(char *arg0, char *arg1);
extern void func_00150600(char *arg0, char *arg1);
extern void func_00150850(unsigned char *a0);
extern void func_00150900(unsigned char *a0, unsigned char *a1);
extern int func_001513B0(char *arg0, char *arg1);
extern unsigned char func_001B17A0(char *self);
extern int anim_advance_time(char *arg0, float dt);
extern void func_001C68C0(char *o);

void func_00150190(char *self, char *ctl) {
    switch (((unsigned char *)self)[5]) {
    case 0:
        func_00150260(self, ctl);
        func_001513B0(self, ctl);
        break;
    case 1:
        func_001504D0(self, ctl);
        break;
    case 2:
        func_00150600(self, ctl);
        break;
    }
    func_00150900((unsigned char *)self, (unsigned char *)ctl);
    *(int *)(ctl + 0) = anim_advance_time(self, *(float *)(ctl + 0x20));
    func_00150850((unsigned char *)self);
    func_001C68C0(self);
    func_001B17A0(self);
    (*(void (**)(char *))(self + 0x4C))(self);
}
