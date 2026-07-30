// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS: per-frame tick for an entity class (state machine "e", per-entity
// anim/state block "st"). Clears st+0x8E, bails unless func_001B2140(e) says the
// entity is active, then re-arms st+0x8E. Rebuilds the two scratch-pad matrices at
// 0x700038A0 / 0x700038B0 (from e+0xB0 and the player position D_00810360), zeroes
// their +0x4 slots, and caches func_001B15D0(m0, m1) -> st+0x5C. Dispatches the
// per-state handler through jtbl_0026D210 on the state byte e[5] (11 states,
// out-of-range = no handler). Post-state common work: e[0] = st[0x88] ? 3 : 1;
// e[0x5D] = st[0x8D] ? 0x81 (+ func_001B4810(e)) : 1; e[0xA] = 0; four sub-updates;
// st+0x30 = anim_advance_time(e, st+0x40); then func_00131ED0/func_001C68C0 and,
// if func_001B17A0(e), the entity's virtual callback at e+0x4C.

extern void func_00102948(void *dst, void *src);
extern float func_001B15D0(void *a, void *b);
extern int func_001B2140(char *a);
extern int func_001B17A0(char *a);
extern void func_001B4810(char *a);
extern void func_00131ED0(char *a);
extern void func_001C68C0(char *a);
extern int anim_advance_time(char *a, float t);
extern void func_0013D850(char *a, char *b);
extern void func_0013D980(char *a, char *b);
extern void func_0013DB70(char *a, char *b);
extern void func_0013DC60(char *a, char *b);
extern void func_0013DD40(char *a, char *b);
extern void func_0013EB90(char *a, char *b);
extern void func_0013E6D0(char *a, char *b);
extern void func_0013EDB0(char *a, char *b);
extern void func_0013F240(char *a, char *b);
extern void func_0013F770(char *a, char *b);
extern void func_00140440(char *a, char *b);
extern void func_00140C30(char *a, char *b);
extern void func_00140F80(char *a, char *b);
extern void func_00141A30(char *a, char *b);
extern void func_00141C90(char *a, char *b);
extern char D_700038A0[];
extern char D_700038B0[];
extern float D_00810360;

void func_0013D600(char *e, char *st) {
    *(char *)(st + 0x8E) = 0;
    if (func_001B2140(e) == 0) {
        return;
    }
    *(char *)(st + 0x8E) = 1;
    func_00102948(D_700038A0, e + 0xB0);
    func_00102948(D_700038B0, &D_00810360);
    *(int *)0x700038B4 = 0;
    *(int *)0x700038A4 = 0;
    *(float *)(st + 0x5C) = func_001B15D0(D_700038A0, D_700038B0);
    *(char *)(st + 0x85) = 0;
    switch (*(unsigned char *)(e + 5)) {
    case 0:
        func_0013D850(e, st);
        break;
    case 1:
        func_0013D980(e, st);
        break;
    case 2:
        func_0013DB70(e, st);
        break;
    case 3:
        func_0013DC60(e, st);
        break;
    case 4:
        func_0013DD40(e, st);
        break;
    case 5:
        func_0013EB90(e, st);
        break;
    case 6:
        func_0013E6D0(e, st);
        break;
    case 7:
        func_0013EDB0(e, st);
        break;
    case 8:
        func_0013F240(e, st);
        break;
    case 9:
        func_0013F770(e, st);
        break;
    case 10:
        func_00140440(e, st);
        break;
    }
    if (*(char *)(st + 0x88) != 0) {
        *(char *)e = 3;
    } else {
        *(char *)e = 1;
    }
    if (*(char *)(st + 0x8D) != 0) {
        *(unsigned char *)(e + 0x5D) = 0x81;
        func_001B4810(e);
    } else {
        *(char *)(e + 0x5D) = 1;
    }
    *(char *)(e + 0xA) = 0;
    func_00141A30(e, st);
    func_00141C90(e, st);
    func_00140C30(e, st);
    func_00140F80(e, st);
    *(int *)(st + 0x30) = anim_advance_time(e, *(float *)(st + 0x40));
    func_00131ED0(e);
    func_001C68C0(e);
    if (func_001B17A0(e) != 0) {
        (*(void (**)(char *))(e + 0x4C))(e);
    }
}
