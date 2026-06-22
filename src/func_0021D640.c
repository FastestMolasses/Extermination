// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Leaf predicate on an actor at p. Reads a state byte at 0x1F0; returns 1 for
// states 0x27, 0x34, 0x35 (the `(unsigned)(state-0x34) < 2` test). Otherwise,
// only when p[4]==1 and p[5] is 0x1F or 0x20, returns 1 iff state==0x33.
// mwcc 2.3.3 reproduces the CW exit/delay-slot lowering (bne ...,END with the
// `paddub v0,zero,zero` return-0 idiom filled into the delay slot) and the
// `addiu v0,a1,-0x34` form that the pinned 991202 build does NOT (991202 reloads
// via `andi v0,a1,0xff`, scoring 83.2%). The nested single-exit shape
// (p[4]==1 wrapping the p[5]/state checks, one trailing `return 0`) is what
// makes mwcc share the return-0 exit instead of emitting per-test dead blocks.
int func_0021D640(unsigned char *p) {
    unsigned char state = p[0x1F0];
    unsigned char b5;
    if (state == 0x27 || (unsigned int)(state - 0x34) < 2) return 1;
    if (p[4] == 1) {
        b5 = p[5];
        if ((b5 == 0x1F || b5 == 0x20) && state == 0x33) return 1;
    }
    return 0;
}
