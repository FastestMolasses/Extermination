// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 90.09% NEARMISS).
// Fixes read from the original instructions:
// - func_001B2140 takes the entity pointer (its definition in src/ shows one
//   parameter): the original leaves arg0 in the first argument register for
//   that call, which is why the state byte lands in the second one. The
//   "register-coloring wall" was this missing argument.
// - The state switch has empty cases 0 and 4 that break, and the 0x52 clear
//   runs after the switch (cases 0, 4 and every unlisted state reach it);
//   only cases 1, 2 and 3 return early.
// - Both switches list their cases in ascending order (mwcc tests them in
//   reverse source order); the substate switch has no default.
//
// Per-entity tick driver dispatched on the global game/menu state byte at
// D_70003B8D. p = arg0 + 0x1F0 is the entity's animation/timer sub-block.
// States 2,3: no-op. State 1: if func_001B2140(arg0) and arg0+4 is set,
// invoke the vtable tick at arg0+0x4C, then return. Any other state: clear
// arg0+0x52, then dispatch on the entity substate at arg0+4 (handlers
// func_00141F00/00142070/001450B0/00145850; substate 3 returns right after
// its handler), then decrement a battery of frame counters in p
// (0x64,0x62,0x73,0x7D,0x72,0x71,0x60) clamped at 0, and when p+0x7E is set
// run func_001B5360 (unless global D_00810700==0xD), then func_001B0D80.
extern unsigned char D_70003B8D;
extern unsigned char D_00810700;
extern int func_001B2140(char *e);
extern int func_00141F00(char *, char *);
extern int func_00142070(char *, char *);
extern int func_001450B0(char *, char *);
extern int func_00145850(char *, char *);
extern int func_001B5360(char *);
extern int func_001B0D80(char *);

void func_00141D20(char *arg0) {
    unsigned char st;
    char *p;
    short hv;
    unsigned char bv;
    char cv;

    st = D_70003B8D;
    p = arg0 + 0x1F0;
    switch (st) {
    case 0:
        break;
    case 1:
        if (func_001B2140(arg0) != 0) {
            if (*(unsigned char *)(arg0 + 4) != 0) {
                (*(void (**)(char *))(arg0 + 0x4C))(arg0);
            }
        }
        return;
    case 2:
    case 3:
        return;
    case 4:
        break;
    }
    *(short *)(arg0 + 0x52) = 0;

    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        func_00141F00(arg0, p);
        break;
    case 1:
        func_00142070(arg0, p);
        break;
    case 2:
        func_001450B0(arg0, p);
        break;
    case 3:
        func_00145850(arg0, p);
        return;
    }

    hv = *(short *)(p + 0x64);
    if (hv != 0) { *(short *)(p + 0x64) = hv - 1; }
    hv = *(short *)(p + 0x62);
    if (hv != 0) { *(short *)(p + 0x62) = hv - 1; }
    bv = *(unsigned char *)(p + 0x73);
    if (bv != 0) { *(unsigned char *)(p + 0x73) = bv - 1; }
    cv = *(char *)(p + 0x7D);
    if (cv != 0) { *(char *)(p + 0x7D) = cv - 1; }
    bv = *(unsigned char *)(p + 0x72);
    if (bv != 0) { *(unsigned char *)(p + 0x72) = bv - 1; }
    bv = *(unsigned char *)(p + 0x71);
    if (bv != 0) { *(unsigned char *)(p + 0x71) = bv - 1; }
    hv = *(short *)(p + 0x60);
    if (hv != 0) { *(short *)(p + 0x60) = hv - 1; }
    if (*(char *)(p + 0x7E) != 0) {
        if (D_00810700 != 0xD) {
            func_001B5360(arg0);
        }
    }
    func_001B0D80(arg0);
}
