// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Thin dispatcher on an actor struct (unsigned char *): calls func_001B1630,
// stores its byte result into +0x1, and if that byte is nonzero calls
// func_001B1B70(actor). Returns the +0x1 byte (re-loaded).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: under
// 991202 the lone residual was the clean-store `beqz` delay-slot nop (wall
// #13) — 991202 fills it, CodeWarrior 2.3.3 leaves the nop. The 2.3.3 build is
// byte-identical (.text cmp vs build/expected) → readable plain C, no asm void.
extern unsigned char func_001B1630(unsigned char *);
extern void func_001B1B70(unsigned char *);

unsigned char func_001B1B30(unsigned char *arg0) {
    arg0[1] = func_001B1630(arg0);
    if (arg0[1] != 0) {
        func_001B1B70(arg0);
    }
    return arg0[1];
}
