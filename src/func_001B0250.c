// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Triple pointer-chase into a per-area table. D_0024D650 is an array of
// pointers indexed by D_00810700 (a level/area id); the resulting pointer is
// itself a pointer-array indexed by D_00810701; that element points at an
// array of 0x30-byte records indexed by D_00810702, whose +0x1C word is
// cached into the global D_008106C8. When the area id is 0xB (area-11) and
// the flag D_00810788 is set, the cached word's bits are rewritten:
// clear mask 0x0E000070 (keep 0xF1FFFF8F) then OR in 0x44.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: under
// 991202 the lone residual is wall #13 (it fills the `bne t0,v1` delay slot
// with the safe li instead of leaving CW's nop). 2.3.3 leaves the nop, so
// this readable C is byte-identical. objdiff 100.0% vs expected.
extern int **D_0024D650[];
extern int D_008106C8;
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_00810788;

void func_001B0250(void) {
    int **p;
    char *q;
    p = D_0024D650[D_00810700];
    q = (char *)p[D_00810701];
    q += D_00810702 * 0x30;
    D_008106C8 = *(int *)(q + 0x1C);
    if (D_00810700 == 0xB && D_00810788 != 0) {
        D_008106C8 &= 0xF1FFFF8F;
        D_008106C8 |= 0x44;
    }
}
