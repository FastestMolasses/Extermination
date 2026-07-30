// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS: per-frame tick for a second entity class (sibling of func_0013D600).
// Clears st+0x8A, bails unless func_001B2140(e) says the entity is active, then
// re-arms st+0x8A and dispatches the per-state handler through jtbl_0026D2C0 on the
// state byte e[5] (10 states, out-of-range = no handler). Post-state common work:
// e[0] = st[0x85] ? 3 : 1; if st[0x86] then e[0x5E] = 0 and e[0x52] |= 1 else
// e[0x5E] = 1; e[0x5D] = st[0x88] ? 0x81 (+ func_001B4810(e)) : 1; e[0xA] = 0;
// four sub-updates. If st[0x83] is set only func_00131ED0(e) runs; otherwise the
// animation is advanced (st+0x20 = anim_advance_time(e, st+0x40)) and, when the low
// byte of that result is 9, sound/event 0x7E6 is fired at range 300.0f, followed by
// func_00131ED0/func_001C68C0. Always ends with func_001B17A0(e) (result unused) and
// the entity's virtual callback at e+0x4C.

extern int func_001B2140(char *e);
extern int func_001B17A0(char *e);
extern void func_001B4810(char *e);
extern void func_00131ED0(char *e);
extern void func_001C68C0(char *e);
extern void func_001FBD50(char *e, int id, int flag, float f);
extern int anim_advance_time(char *e, float step);
extern void func_00147960(char *e, char *st);
extern void func_00147B50(char *e, char *st);
extern void func_00148130(char *e, char *st);
extern void func_00148400(char *e, char *st);
extern void func_00148520(char *e, char *st);
extern void func_001486E0(char *e, char *st);
extern void anim_matrix_multi(char *e, char *st);
extern void func_00149B50(char *e, char *st);
extern void func_0014A0D0(char *e, char *st);
extern void func_0014A350(char *e, char *st);
extern void func_0014AE80(char *e, char *st);
extern void func_0014AFA0(char *e, char *st);
extern void func_0014BE20(char *e, char *st);
extern void func_0014C0B0(char *e, char *st);

void func_00147700(char *e, char *st) {
    *(char *)(st + 0x8A) = 0;
    if (func_001B2140(e) == 0) {
        return;
    }
    *(char *)(st + 0x8A) = 1;
    switch (*(unsigned char *)(e + 5)) {
    case 0:
        func_00147960(e, st);
        break;
    case 1:
        func_00147B50(e, st);
        break;
    case 2:
        func_00148130(e, st);
        break;
    case 3:
        func_00148400(e, st);
        break;
    case 4:
        func_00148520(e, st);
        break;
    case 5:
        func_001486E0(e, st);
        break;
    case 6:
        anim_matrix_multi(e, st);
        break;
    case 7:
        func_00149B50(e, st);
        break;
    case 8:
        func_0014A0D0(e, st);
        break;
    case 9:
        func_0014A350(e, st);
        break;
    }
    if (*(char *)(st + 0x85) != 0) {
        *(char *)e = 3;
    } else {
        *(char *)e = 1;
    }
    if (*(char *)(st + 0x86) != 0) {
        *(char *)(e + 0x5E) = 0;
        *(unsigned short *)(e + 0x52) |= 1;
    } else {
        *(char *)(e + 0x5E) = 1;
    }
    if (*(char *)(st + 0x88) != 0) {
        *(unsigned char *)(e + 0x5D) = 0x81;
        func_001B4810(e);
    } else {
        *(char *)(e + 0x5D) = 1;
    }
    *(char *)(e + 0xA) = 0;
    func_0014C0B0(e, st);
    func_0014BE20(e, st);
    func_0014AE80(e, st);
    func_0014AFA0(e, st);
    if (*(char *)(st + 0x83) != 0) {
        func_00131ED0(e);
    } else {
        *(int *)(st + 0x20) = anim_advance_time(e, *(float *)(st + 0x40));
        if (*(unsigned char *)(st + 0x20) == 9) {
            func_001FBD50(e, 0x7E6, 0, 300.0f);
        }
        func_00131ED0(e);
        func_001C68C0(e);
    }
    func_001B17A0(e);
    (*(void (**)(char *))(e + 0x4C))(e);
}
