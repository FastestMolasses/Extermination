// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Per-frame update for one actor class, called as (e, st) with st the
// per-actor state block. Runs the behaviour handler for the state byte e[5]
// (0 func_00140760, 1 func_001408D0, 2 func_00140A90; other values run
// none), then the fixed chain: func_00140C30, func_00140F80,
// st+0x30 = anim_advance_time(e, st+0x40), func_00131ED0, func_001C68C0,
// func_001B17A0, and finally the actor's hook at e+0x4C. The hook is called
// unconditionally here (func_001B17A0's result is ignored), unlike
// func_0013D600, which gates the hook on it.
extern void func_00140760(char *e, char *st);
extern void func_001408D0(char *e, char *st);
extern void func_00140A90(char *e, char *st);
extern void func_00140C30(char *e, char *st);
extern void func_00140F80(char *e, char *st);
extern int anim_advance_time(char *e, float t);
extern void func_00131ED0(char *e);
extern void func_001C68C0(char *e);
extern int func_001B17A0(char *e);

void func_00140690(char *e, char *st) {
    switch (*(unsigned char *)(e + 5)) {
    case 0:
        func_00140760(e, st);
        break;
    case 1:
        func_001408D0(e, st);
        break;
    case 2:
        func_00140A90(e, st);
        break;
    }
    func_00140C30(e, st);
    func_00140F80(e, st);
    *(int *)(st + 0x30) = anim_advance_time(e, *(float *)(st + 0x40));
    func_00131ED0(e);
    func_001C68C0(e);
    func_001B17A0(e);
    (*(void (**)(char *))(e + 0x4C))(e);
}
