// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Per-tick update for a scripted variant of the func_0014D470 actor: dispatches
// sub-state self+5 (0 -> func_0014E420, 1 -> func_0014E4F0), then the same tail
// as func_0014D470 without the func_0014E9C0 step: func_0014E670, func_0014E890,
// copy ctx+0x28 into (self+0x138)+0x70, anim_advance_time(self, ctx+0x20) into
// ctx+0, func_001C68C0, func_001B17A0, and the self+0x4C hook.
extern void func_0014E420(char *arg0, char *arg1);
extern void func_0014E4F0(char *arg0, char *arg1);
extern void func_0014E670(char *arg0, char *arg1);
extern void func_0014E890(char *arg0, char *arg1);
extern int anim_advance_time(char *self, float dt);
extern void func_001C68C0(char *o);
extern unsigned char func_001B17A0(char *self);

void func_0014E370(char *self, char *ctx) {
    switch (*(unsigned char *)(self + 5)) {
    case 0:
        func_0014E420(self, ctx);
        break;
    case 1:
        func_0014E4F0(self, ctx);
        break;
    }
    func_0014E670(self, ctx);
    func_0014E890(self, ctx);
    *(float *)(*(char **)(self + 0x138) + 0x70) = *(float *)(ctx + 0x28);
    *(int *)ctx = anim_advance_time(self, *(float *)(ctx + 0x20));
    func_001C68C0(self);
    func_001B17A0(self);
    (*(void (**)(char *))(self + 0x4C))(self);
}
