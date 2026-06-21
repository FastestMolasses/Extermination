// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Picks status/sound id 0x14E or 0x14F from the byte flag arg0[0x234] and
// calls func_001FBD50(arg0, 300.0f, id, 0). func_001FBD50 ignores its first
// GPR slot (a0) -- it reads a1 (id), a2 (the 0) and the float in f12; arg0 is
// already live in a0 from this function's own argument, so passing it as the
// (ignored) leading param both flows through with no reload AND advances the
// integer-arg slots so the id lands in a1 / the 0 in a2 while the float
// constant 300.0f (0x43960000) occupies fa0. The float is rematerialized via
// a duplicate `lui v0,0x4396` in the else-path branch-delay slot -- CW codegen
// the pinned 991202 build drops (walled at 95%); mwcc 2.3.3
// (mwcps2-2.3.3-000906) keeps it and is byte-identical. Verified objdiff 100%
// vs build/expected/func_0021D490.o.
extern void func_001FBD50(char *, float, int, int);

void func_0021D490(char *arg0) {
    if (*(unsigned char *)(arg0 + 0x234) == 0) {
        func_001FBD50(arg0, 300.0f, 0x14E, 0);
    } else {
        func_001FBD50(arg0, 300.0f, 0x14F, 0);
    }
}
