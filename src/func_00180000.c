// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (listing-identical to the concatenated pair), matching wave
// 2026-10-28 (was an asm body split across two placeholders).
//
// SPLIT PAIR: splat cut this function at the 0x00180000 boundary; the
// placeholder func_00180000 holds only the stack adjust and func_00180004 the
// rest (no jal reaches 0x00180004). This is the whole function, size 0x40.
// Integrating it needs the split-pair recipe: name func_00180000 with
// size 0x40 in config/symbol_addrs.txt, re-split, drop the func_00180004
// placeholder and its .s.
//
// Plays the jump clip for the entity's side: clip 0x99 when e[0x2F1] == 0,
// else 0x9B, flags 1, passing the caller's blend through (callers in
// func_00168050 pass 4.0 and 1.0).
extern int func_001749A0(unsigned char *e, short clip, int flags, float blend);

void func_00180000(unsigned char *e, float blend) {
    if (e[0x2F1] == 0) {
        func_001749A0(e, 0x99, 1, blend);
    } else {
        func_001749A0(e, 0x9B, 1, blend);
    }
}
