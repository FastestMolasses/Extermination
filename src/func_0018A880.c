// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// MATCH NOTE (m3-matching lane, 61.5% -> 100%): mwcc 2.3.3 plus stores in the
// target's order (+3, +0xD, then the handler word at +0x10). Parameters are
// int: the body only sb-stores their low bytes (paddub rd,rs,zero is mwcc's
// register move, not byte evidence), so it cannot decide the width, and both
// callers (func_0015C310, func_0015C420) pass constants and declare
// (int, int). int and unsigned char both measure 100% here.
//
// Semantics (target 0x0018A880): get a node pointer from func_001AFA90(1); if one
// is returned, set its byte +3 = a, byte +0xD = b and its word +0x10 to the
// handler func_0018A6B0. Returns the node (or 0).

extern void func_0018A6B0(void);
extern char *func_001AFA90(int);

char *func_0018A880(int a, int b) {
    char *p;

    p = func_001AFA90(1);
    if (p != 0) {
        p[3] = a;
        p[0xD] = b;
        *(void (**)(void))(p + 0x10) = func_0018A6B0;
    }
    return p;
}
