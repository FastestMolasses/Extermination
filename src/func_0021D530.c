// NEARMISS func_0021D530  (vram 0x0021D530, 0xC4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.67% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Delay-slot scheduling (inverse of the clean-store-nop case). The original CodeWarrior build leaves a nop in the first beqz delay slot (the D_00810E70 & *0x70003B7E test); BOTH mwcc builds (233 and 991202) speculatively hoist the branch-target's first instruction -- the second HW-register address ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Per-actor state setter, gated on event-flag global D_00810E70 ANDed with two IOP
// HW-register words (0x70003B7E then 0x70003B7C, absolute reads). Always sets
// actor+4 = 1 first. If actor+0x237 is already set: latch state 0x1C / anim 0x30
// and return. Else, depending on which HW bit is live: state 0x1E/anim 0x32 (0x3B7E)
// or state 0x1D/anim 0x31 (0x3B7C), each also zeroing actor+0x1F1 and setting the
// 'busy' byte actor+0x317 = 1. If neither bit is live: clear actor+0x25C, call
// func_0017C540(actor, flags), and clear actor+0x317.
//
// NEARMISS 98.67% vs mwcc 2.3.3 (mwcps2-2.3.3-000906); 991202 = 82.2%. Keys that
// got the body byte-exact: `int flags` (not u16) keeps the masked event word full-
// width in $a1 across both ANDs (avoids a re-truncating andi), and the real
// func_0017C540 signature is (actor, flags) -- the trailing constant 1 lives in
// $a2 for the actor+4 / actor+0x317 stores. Sole residual: the target leaves a nop
// in the first beqz delay slot while both mwcc builds hoist the second HW address
// `lui at,0x7000` into it -- a delay-slot scheduling artifact 2.3.3 does not fix.
extern unsigned short D_00810E70;
extern void func_0017C540(char *p, int flags);

void func_0021D530(char *arg0) {
    int flags;

    *(char *)(arg0 + 4) = 1;
    if (*(unsigned char *)(arg0 + 0x237) != 0) {
        *(char *)(arg0 + 5) = 0x1C;
        *(char *)(arg0 + 6) = 0;
        *(char *)(arg0 + 0x1F0) = 0x30;
        return;
    }
    flags = D_00810E70;
    if (flags & *(unsigned short *)0x70003B7E) {
        *(char *)(arg0 + 5) = 0x1E;
        *(char *)(arg0 + 6) = 0;
        *(char *)(arg0 + 0x1F0) = 0x32;
        *(char *)(arg0 + 0x1F1) = 0;
        *(char *)(arg0 + 0x317) = 1;
        return;
    }
    if (flags & *(unsigned short *)0x70003B7C) {
        *(char *)(arg0 + 5) = 0x1D;
        *(char *)(arg0 + 6) = 0;
        *(char *)(arg0 + 0x1F0) = 0x31;
        *(char *)(arg0 + 0x1F1) = 0;
        *(char *)(arg0 + 0x317) = 1;
        return;
    }
    *(char *)(arg0 + 0x25C) = 0;
    func_0017C540(arg0, flags);
    *(char *)(arg0 + 0x317) = 0;
}
