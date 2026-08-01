// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS: per-frame driver for one spawned effect instance (arg0).
//   arg0+0x04 : state byte (0 = spawn, 1 = run, 2/3 = teardown)
//   arg0+0x0D : effect id (0x0B..0x1A used by the lifetime table)
//   arg0+0xB0 : translation      arg0+0xC0 : rotation
//   arg0+0xD0 : world matrix built from them
//   arg0+0x104: a scalar bumped by 30.0 for id 0x14
//   arg0+0x1F0: timer block { +0 start stamp, +4 lifetime, +8 ticks }
// State 0 (spawn): unless the id is 0x10 or 0x13 (which keep whatever transform
//   they were given), rebuild the effect matrix -- func_001029C0 initialises
//   +0xD0, func_00102C58 folds in the +0xC0 rotation and func_00102918 the
//   +0xB0 translation; id 0x14 instead advances +0x104 by 30.0f. Then look up
//   the lifetime in frames by id (0x1E for 0x10/0x19/0x1A, 0x3C for
//   0x0B-0x0E/0x13/0x15/0x18, 0x78 for 0x0F, 0xB4 for 0x14, 1 otherwise --
//   a jtbl_0026EA40 dispatch over id-0x0B), stamp the start time from
//   func_00122BB8(), zero the tick counter and advance to state 1.
// State 1 (run): tick the effect with func_001F4190(arg0, timer,
//   &D_0025A350[id * 0x60]) -- D_0025A350 is the 0x60-byte-per-id effect
//   parameter table -- then bump the tick counter and move to state 3 once it
//   passes the lifetime.
// States 2 and 3 (teardown): hand off to func_001AFC10(arg0).
//
// MATCH NOTES: the jump table is .rodata of this same TU (build/jtblrodata).
// Two shapes were load-bearing: (a) the lifetime store must be written INSIDE
// each case (`*(int *)(st + 4) = N; break;`), not through a shared temp --
// that is what puts `sw v0,0x4(s0)` in each case's branch delay slot and lets
// the out-of-range path use the beqzl annulled `addiu v0,zero,1`; (b) the
// state-1 comparison must be written counter-first (`ticks > lifetime`) to get
// the target's `lw a0,0x8(s0); lw v1,0x4(s0); slt at,v1,a0` operand order.
// 991202 caps at 89.71% on the case-block scheduling.

extern void func_001029C0();
extern void func_00102C58(char *dst, char *src, char *arg);
extern void func_00102918(char *dst, char *src, char *arg);
extern int func_00122BB8(void);
extern void func_001AFC10();
extern void func_001F4190();
extern char D_0025A350[];

void func_001F4840(char *arg0) {
    char *st;

    st = arg0 + 0x1F0;
    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        switch (*(unsigned char *)(arg0 + 0xD)) {
        case 0x10:
        case 0x13:
            break;
        case 0x14:
            *(float *)(arg0 + 0x104) += 30.0f;
            break;
        default:
            func_001029C0(arg0 + 0xD0);
            func_00102C58(arg0 + 0xD0, arg0 + 0xD0, arg0 + 0xC0);
            func_00102918(arg0 + 0xD0, arg0 + 0xD0, arg0 + 0xB0);
            break;
        }
        switch (*(unsigned char *)(arg0 + 0xD)) {
        case 0x10:
        case 0x19:
        case 0x1A:
            *(int *)(st + 4) = 0x1E;
            break;
        case 0xB:
        case 0xC:
        case 0xD:
        case 0xE:
        case 0x13:
        case 0x15:
        case 0x18:
            *(int *)(st + 4) = 0x3C;
            break;
        case 0xF:
            *(int *)(st + 4) = 0x78;
            break;
        case 0x14:
            *(int *)(st + 4) = 0xB4;
            break;
        default:
            *(int *)(st + 4) = 1;
            break;
        }
        *(int *)(st + 0) = func_00122BB8();
        *(int *)(st + 8) = 0;
        *(unsigned char *)(arg0 + 4) = 1;
        break;
    case 1:
        func_001F4190(arg0, st, &D_0025A350[*(unsigned char *)(arg0 + 0xD) * 0x60]);
        *(int *)(st + 8) += 1;
        if (*(int *)(st + 8) > *(int *)(st + 4)) {
            *(unsigned char *)(arg0 + 4) = 3;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
