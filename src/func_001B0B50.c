// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Reads flag word D_008106C8; sets status byte D_008106BE: bit0 -> 1,
// else bit1 -> 0x81, else 0. The 991202 build cannot match (87.0): it
// emits a 2.3.1-style dead `andi v1,a0,2` and a pre-hoisted beqz delay
// slot. mwcc 2.3.3 reproduces CW's codegen byte-for-byte from plain C.
extern int D_008106C8;
extern unsigned char D_008106BE;

void func_001B0B50(void) {
    int f;
    f = D_008106C8;
    if (f & 0x1) {
        D_008106BE = 1;
    } else if (f & 0x2) {
        D_008106BE = 0x81;
    } else {
        D_008106BE = 0;
    }
}
