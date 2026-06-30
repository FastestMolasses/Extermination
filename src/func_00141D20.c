// NEARMISS func_00141D20  (vram 0x00141D20, 0x1D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.09% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring + switch-dispatch artifact (identical 90.1% on both builds, so not a clean-store-nop case 2.3.3 can fix). Target keeps the function arg in a0 live across the whole body and colors the dispatch byte D_70003B8D into a1; mwcc moves arg0 into s1 early then reuses the now-dead a0 to ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 90.1% (both mwcc builds). Logic fully recovered; residuals are
// register coloring (target holds the dispatch byte in a1 with arg0 live in a0;
// mwcc reuses a0) and the outer switch testing case 0 last vs after case 4.
//
// Per-entity tick driver dispatched on the global game/menu state byte at
// D_70003B8D. p = arg0 + 0x1F0 is the entity's animation/timer sub-block.
// State 4: clear arg0+0x52 then fall to the substate handler. States 2,3: no-op.
// State 1: if func_001B2140() and arg0+4 is set, invoke the vtable tick at
// arg0+0x4C, then return. State 0/default: clear arg0+0x52 and fall through.
// The fall-through dispatches on the entity substate at arg0+4 (handlers
// func_00141F00/00142070/001450B0/00145850), then decrements a battery of
// frame counters in p (0x64,0x62,0x73,0x7D,0x72,0x71,0x60) clamped at 0, and
// when p+0x7E is set runs func_001B5360 (unless global D_00810700==0xD) then
// func_001B0D80.
extern unsigned char D_70003B8D;
extern unsigned char D_00810700;
extern int func_001B2140();
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
    case 1:
        if (func_001B2140() != 0) {
            if (*(unsigned char *)(arg0 + 4) != 0) {
                (*(void (**)(char *))(arg0 + 0x4C))(arg0);
            }
        }
        return;
    case 2:
    case 3:
        return;
    case 0:
    case 4:
    default:
        *(short *)(arg0 + 0x52) = 0;
        break;
    }

    switch (*(unsigned char *)(arg0 + 4)) {
    case 3:
        func_00145850(arg0, p);
        return;
    case 2:
        func_001450B0(arg0, p);
        break;
    case 1:
        func_00142070(arg0, p);
        break;
    case 0:
        func_00141F00(arg0, p);
        break;
    default:
        break;
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
