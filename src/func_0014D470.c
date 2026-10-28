// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Per-tick update for an actor kind (sibling of func_0014E370/func_0014EDD0).
// Runs func_0014E7A0, then dispatches the sub-state byte self+5 (0..4) to
// func_0014D5F0 / func_0014D7C0 / func_0014DC30 / func_0014E050 / func_0014E2C0.
// Sets the draw/active byte self+0 to 2 while ctx+0x52 (a short timer) is
// nonzero, else 1. If ctx+0x58 is set, sets bit 0x80 of self+0x5D and calls
// func_001B4810(self); otherwise clears that bit. Then the shared body tail:
// func_0014E670 (random tilt), func_0014E890 (scale pulse), copies ctx+0x28 into
// the float at (self+0x138)+0x70, func_0014E9C0, advances the animation clip
// (anim_advance_time(self, ctx+0x20), result stored to ctx+0), func_001C68C0,
// func_001B17A0 and finally the per-actor hook at self+0x4C.
extern void func_0014E7A0(char *arg0, char *arg1);
extern void func_0014D5F0(char *self, char *ctx);
extern void func_0014D7C0(char *arg0, char *arg1);
extern void func_0014DC30(char *p, char *e);
extern void func_0014E050(char *arg0, char *arg1);
extern void func_0014E2C0(unsigned char *p, char *q);
extern void func_0014E670(char *arg0, char *arg1);
extern void func_0014E890(char *arg0, char *arg1);
extern int func_0014E9C0(unsigned char *e, unsigned char *p);
extern void func_001B4810(unsigned char *self);
extern int anim_advance_time(char *self, float dt);
extern void func_001C68C0(char *o);
extern unsigned char func_001B17A0(char *self);

void func_0014D470(char *self, char *ctx) {
    func_0014E7A0(self, ctx);
    switch (*(unsigned char *)(self + 5)) {
    case 0:
        func_0014D5F0(self, ctx);
        break;
    case 1:
        func_0014D7C0(self, ctx);
        break;
    case 2:
        func_0014DC30(self, ctx);
        break;
    case 3:
        func_0014E050(self, ctx);
        break;
    case 4:
        func_0014E2C0((unsigned char *)self, ctx);
        break;
    }
    if (*(short *)(ctx + 0x52) != 0) {
        *(char *)(self + 0) = 2;
    } else {
        *(char *)(self + 0) = 1;
    }
    if (*(char *)(ctx + 0x58) != 0) {
        *(unsigned char *)(self + 0x5D) |= 0x80;
        func_001B4810((unsigned char *)self);
    } else {
        *(unsigned char *)(self + 0x5D) &= 0x7F;
    }
    func_0014E670(self, ctx);
    func_0014E890(self, ctx);
    *(float *)(*(char **)(self + 0x138) + 0x70) = *(float *)(ctx + 0x28);
    func_0014E9C0((unsigned char *)self, (unsigned char *)ctx);
    *(int *)ctx = anim_advance_time(self, *(float *)(ctx + 0x20));
    func_001C68C0(self);
    func_001B17A0(self);
    (*(void (**)(char *))(self + 0x4C))(self);
}
