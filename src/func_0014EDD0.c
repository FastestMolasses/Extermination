// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Active update (state 1 of func_0014EC00). Dispatches sub-state self+5:
// 0 -> func_0014EEF0, 1 -> func_0014F390, 2 -> func_0014FBC0. Then
// func_001B4810(self), func_00150900, func_001509A0 and func_001513B0 (each
// (self, ctx)); sets self+0 = 2 when the height self+0xB4 is at or below -100,
// else 1; then anim_advance_time(self, ctx+0x20) into ctx+0, func_00150850,
// func_001C68C0, func_001B17A0 and the self+0x4C hook.
extern void func_0014EEF0(char *self, char *ctx);
extern void func_0014F390(unsigned char *p, unsigned char *e);
extern void func_0014FBC0(unsigned char *p, unsigned char *e);
extern void func_001B4810(unsigned char *self);
extern void func_00150900(unsigned char *a0, unsigned char *a1);
extern void func_001509A0(char *arg0, char *arg1);
extern int func_001513B0(char *arg0, char *arg1);
extern int anim_advance_time(char *self, float dt);
extern void func_00150850(unsigned char *a0);
extern void func_001C68C0(char *o);
extern unsigned char func_001B17A0(char *self);

void func_0014EDD0(char *self, char *ctx) {
    switch (*(unsigned char *)(self + 5)) {
    case 0:
        func_0014EEF0(self, ctx);
        break;
    case 1:
        func_0014F390((unsigned char *)self, (unsigned char *)ctx);
        break;
    case 2:
        func_0014FBC0((unsigned char *)self, (unsigned char *)ctx);
        break;
    }
    func_001B4810((unsigned char *)self);
    func_00150900((unsigned char *)self, (unsigned char *)ctx);
    func_001509A0(self, ctx);
    func_001513B0(self, ctx);
    if (*(float *)(self + 0xB4) <= -100.0f) {
        *(char *)(self + 0) = 2;
    } else {
        *(char *)(self + 0) = 1;
    }
    *(int *)ctx = anim_advance_time(self, *(float *)(ctx + 0x20));
    func_00150850((unsigned char *)self);
    func_001C68C0(self);
    func_001B17A0(self);
    (*(void (**)(char *))(self + 0x4C))(self);
}
