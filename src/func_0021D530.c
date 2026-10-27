// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.67% NEARMISS).
// Fixes read from the original instructions:
// - The two scratchpad pad masks (0x70003B7E, 0x70003B7C) are relocated externs
//   (idiom-32), not literal addresses. As literals mwcc treated the second address
//   as a constant and speculated its upper half into the first branch's delay slot,
//   which the original leaves empty. Both addresses are already in build.py's global
//   _SPAD_SYMS, so no `// SPAD:` line is needed (twins func_001607D0 / func_001696A0
//   spell them the same way).

// Per-actor state setter, gated on event-flag global D_00810E70 ANDed with two
// scratchpad pad-mask words (D_70003B7E then D_70003B7C). Always sets
// actor+4 = 1 first. If actor+0x237 is already set: latch state 0x1C / anim 0x30
// and return. Else, depending on which mask bit is live: state 0x1E/anim 0x32 (0x3B7E)
// or state 0x1D/anim 0x31 (0x3B7C), each also zeroing actor+0x1F1 and setting the
// 'busy' byte actor+0x317 = 1. If neither bit is live: clear actor+0x25C, call
// func_0017C540(actor, flags), and clear actor+0x317.
// `int flags` (not u16) keeps the masked event word full-width across both ANDs,
// and func_0017C540's real signature is (actor, flags).
extern unsigned short D_00810E70;
extern unsigned short D_70003B7E;
extern unsigned short D_70003B7C;
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
    if (flags & D_70003B7E) {
        *(char *)(arg0 + 5) = 0x1E;
        *(char *)(arg0 + 6) = 0;
        *(char *)(arg0 + 0x1F0) = 0x32;
        *(char *)(arg0 + 0x1F1) = 0;
        *(char *)(arg0 + 0x317) = 1;
        return;
    }
    if (flags & D_70003B7C) {
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
