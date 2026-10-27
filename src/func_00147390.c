// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.98% NEARMISS).
// Fixes read from the original instructions:
//  - The outer mode switch is spelled like byte-matched twin func_0013D2D0:
//    case 0 break; case 1 handler; cases 2/3 return; case 4 break (in that
//    order). That gives the target's descending compare chain.
//  - func_001B2140 takes the entity pointer (a0 is still arg0 at the call),
//    not the constant 1; the mode-1 gate is written as two early returns.
//  - func_0014AE50 is declared with two parameters because the call site
//    leaves sub in a1 (its definition in src/ takes one; the callee ignores it).
//
// Per-frame entity update dispatcher (substate object at arg0+0x1F0). Outer switch
// on the global game-mode byte at 0x70003B8D: modes 2/3 do nothing; mode 1 runs the
// gated virtual callback (if func_001B2140(arg0) and arg0[4]!=0, invoke the function
// pointer at arg0+0x4C); modes 0/4 fall through (any other mode also reaches it) to the per-substate
// update. The body clears arg0+0x52, runs func_001B0D80(arg0), copies two transform
// blocks (func_00102948 into D_700038A0 from arg0+0xB0, and into D_700038B0 from
// &D_00810360), zeroes the absolute words 0x700038B4 / 0x700038A4, and stores
// func_001B15D0(D_700038A0, D_700038B0) into (sub+0x68) as a float. Then an inner
// switch on arg0[4] dispatches the substate handler (0->func_001475A0,
// 1->func_00147700, 2->func_0014A620, 3->func_0014AE50 then return). Cases 0/1/2
// then run the timer wave: decrement-clamp-at-zero on sub+0x38 (int), +0x8C (byte),
// +0x74 (short), +0x70 (short), +0x89 (byte), +0x8E (byte), +0x85 (byte); clear
// arg0+0x54; and if sub+0x8A is set, call func_001B5360(arg0). Near-twin of
// func_001383C0.
extern void func_001475A0(char *p, char *q);
extern void func_00147700(char *p, char *q);
extern void func_0014A620(char *p, char *q);
extern void func_0014AE50(char *p, char *q);
extern void func_001B0D80(char *p);
extern void func_00102948(void *dst, void *src);
extern float func_001B15D0(void *a, void *b);
extern int func_001B2140(char *e);
extern void func_001B5360(char *p);

extern char D_700038A0[];
extern char D_700038B0[];
extern float D_00810360;

void func_00147390(char *arg0) {
    char *sub;
    int t;
    char c;
    short h;

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

    *(short *)(arg0 + 0x52) = 0;
    func_001B0D80(arg0);
    func_00102948(D_700038A0, arg0 + 0xB0);
    func_00102948(D_700038B0, &D_00810360);
    *(int *)0x700038B4 = 0;
    *(int *)0x700038A4 = 0;
    *(float *)(sub + 0x68) = func_001B15D0(D_700038A0, D_700038B0);

    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        func_001475A0(arg0, sub);
        break;
    case 1:
        func_00147700(arg0, sub);
        break;
    case 2:
        func_0014A620(arg0, sub);
        break;
    case 3:
        func_0014AE50(arg0, sub);
        return;
    }

    t = *(int *)(sub + 0x38);
    if (t != 0) {
        *(int *)(sub + 0x38) = t - 1;
    }
    c = *(char *)(sub + 0x8C);
    if (c != 0) {
        *(char *)(sub + 0x8C) = c - 1;
    }
    h = *(short *)(sub + 0x74);
    if (h != 0) {
        *(short *)(sub + 0x74) = h - 1;
    }
    h = *(short *)(sub + 0x70);
    if (h != 0) {
        *(short *)(sub + 0x70) = h - 1;
    }
    c = *(char *)(sub + 0x89);
    if (c != 0) {
        *(char *)(sub + 0x89) = c - 1;
    }
    c = *(char *)(sub + 0x8E);
    if (c != 0) {
        *(char *)(sub + 0x8E) = c - 1;
    }
    c = *(char *)(sub + 0x85);
    if (c != 0) {
        *(char *)(sub + 0x85) = c - 1;
    }
    *(short *)(arg0 + 0x54) = 0;
    if (*(char *)(sub + 0x8A) != 0) {
        func_001B5360(arg0);
    }
}
