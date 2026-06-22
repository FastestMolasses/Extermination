// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-tick countdown on the global object at D_00810E40, gated by the flag
// byte D_00810E56. When the flag is set, decrement the short timer at +0x28:
// if it has already hit 0, fire func_001B6250 (the expiry/fire action);
// otherwise store timer-1 back. Returns nothing.
//
// 233-only: under 991202 the lone residual is the clean-store delay-slot nop
// of the bnel/sh idiom (wall #13) plus the reloc-pair %hi/%lo materialization
// of &D_00810E40; mwcc 2.3.3 (mwcps2-2.3.3-000906) reproduces CW's lowering
// byte-identically. Verified objdiff 100.0 vs build/expected/func_001B5B70.o.
extern char D_00810E40;
extern unsigned char D_00810E56;
extern void func_001B6250(void);

void func_001B5B70(void) {
    char *base;
    short v;

    base = &D_00810E40;
    if (D_00810E56 != 0) {
        v = *(short *)(base + 0x28);
        if (v == 0) {
            func_001B6250();
        } else {
            *(short *)(base + 0x28) = v - 1;
        }
    }
}
