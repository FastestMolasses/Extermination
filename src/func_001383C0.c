// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.63% NEARMISS).
// Fixes read from the original instructions:
//  - The outer mode switch is spelled like byte-matched twins func_00147390 /
//    func_0013D2D0: case 0 break; case 1 handler; cases 2/3 return; case 4
//    break (in that order). That gives the target's descending compare chain
//    (the old "switch-lowering artifact" verdict was a wrong spelling).
//  - func_001B2140 takes the entity pointer (a0 is still arg0 at the call),
//    not the constant 1; the mode-1 gate is written as two early returns.
//
// Per-frame update dispatcher for an entity (substate object at arg0+0x1F0).
// First switches on the global game-mode byte at 0x70003B8D: modes 2/3 do
// nothing; mode 1 runs the gated callback (if func_001B2140(arg0) and arg0+4 != 0,
// invoke the virtual at arg0+0x4C); modes 0/4 (any other mode also reaches it)
// fall through to the per-substate update, which dispatches on the substate byte
// at arg0+4 (0->func_00138540, 1->func_001386E0, 2->func_0013B350,
// 3->func_0013B9A0 then return). After the 0/1/2 handlers it decrements the
// 8-bit timer at +0x85 and the 16-bit timer at +0x34 (clamped at 0), runs
// func_001B0D80, and if the +0x87 flag is set and the menu/global state
// (D_00810700 != 0x13 || D_00810701 != 0) permits, calls func_001B5360.
// Near-twin of func_00147390.
extern void func_00138540(char *p, char *q);
extern void func_001386E0(char *p, char *q);
extern void func_0013B350(char *p, char *q);
extern void func_0013B9A0(char *p, char *q);
extern void func_001B0D80(char *p);
extern int func_001B2140(char *e);
extern void func_001B5360(char *p);
extern unsigned char D_00810700;
extern unsigned char D_00810701;

void func_001383C0(char *arg0) {
    char *sub;
    unsigned char st;
    char c;
    unsigned short h;

    sub = arg0 + 0x1F0;
    switch (*(unsigned char *)0x70003B8D) {
    case 0:
        break;
    case 1:
        if (func_001B2140(arg0) == 0) return;
        if (*(unsigned char *)(arg0 + 4) == 0) return;
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        return;
    case 2:
    case 3:
        return;
    case 4:
        break;
    }
    st = *(unsigned char *)(arg0 + 4);
    switch (st) {
    case 0:
        func_00138540(arg0, sub);
        break;
    case 1:
        func_001386E0(arg0, sub);
        break;
    case 2:
        func_0013B350(arg0, sub);
        break;
    case 3:
        func_0013B9A0(arg0, sub);
        return;
    }
    c = *(char *)(sub + 0x85);
    if (c != 0) {
        *(char *)(sub + 0x85) = c - 1;
    }
    h = *(unsigned short *)(sub + 0x34);
    if (h != 0) {
        *(unsigned short *)(sub + 0x34) = h - 1;
    }
    func_001B0D80(arg0);
    if (*(char *)(sub + 0x87) != 0 && (D_00810700 != 0x13 || D_00810701 != 0)) {
        func_001B5360(arg0);
    }
}
