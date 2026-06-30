// NEARMISS func_001383C0  (vram 0x001383C0, 0x178 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.63% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Outer mode-switch dispatch lowering: target hoists the beqz (mode 0) test ahead of the beq ,1 test and assigns the compare-constant scratch register differently (a0 vs v0). Switch-lowering / comparison-ordering scheduling artifact; case reorderings did not reproduce the target's exact dispatch se...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 98.6% (identical on 991202 and 2.3.3). Per-frame update dispatcher for
// an entity (substate object at arg0+0x1F0). First switches on the global game-mode
// byte at 0x70003B8D: modes 2/3 do nothing; mode 1 runs the gated callback
// (if func_001B2140() and arg0+4 != 0, invoke the virtual at arg0+0x4C); modes 0/4
// (and default) fall through to the per-substate update, which dispatches on the
// substate byte at arg0+4 (0->func_00138540, 1->func_001386E0, 2->func_0013B350,
// 3->func_0013B9A0 then return). After the 0/1/2 handlers it decrements the
// 8-bit timer at +0x85 and the 16-bit timer at +0x34 (clamped at 0), runs
// func_001B0D80, and if the +0x87 flag is set and the menu/global state
// (D_00810700 != 0x13 || D_00810701 != 0) permits, calls func_001B5360.
//
// Body, both switches, both timers, and all call targets are byte-correct. The
// sole residual is mwcc's switch-dispatch lowering for the outer mode switch:
// the target hoists the `beqz` (mode 0) test ahead of the `beq ,1` test and uses
// a different scratch register for the compare constants. This is a switch-
// lowering / comparison-ordering scheduling artifact; reshaping the case order
// could not steer mwcc onto the target's exact dispatch sequence.
extern void func_00138540(char *p, char *q);
extern void func_001386E0(char *p, char *q);
extern void func_0013B350(char *p, char *q);
extern void func_0013B9A0(char *p, char *q);
extern void func_001B0D80(char *p);
extern int func_001B2140(unsigned char m);
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
    default:
    case 0:
    case 4:
        break;
    case 1:
        if (func_001B2140(1) != 0 && *(unsigned char *)(arg0 + 4) != 0) {
            (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        }
    case 2:
    case 3:
        return;
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
