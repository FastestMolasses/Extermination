// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Calls func_001F0060 with a magic id (0x80000061 or 0x80000062) selected by the
// byte at a0[0x234], second arg always 0 (paddub a1,zero,zero). 2.3.3 reproduces the
// 2.3.1 layout (dead lui v0,0x8000 + hoisted lui in the bnez delay slot) that the
// 991202 build omits.
extern void func_001F0060(int, int);

void func_0021D4E0(unsigned char *a0) {
    if (a0[0x234] == 0) {
        func_001F0060(0x80000061, 0);
    } else {
        func_001F0060(0x80000062, 0);
    }
}
