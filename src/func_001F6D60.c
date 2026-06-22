// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Area/sub-area dispatch table selector. Packs the current area id
// (D_00810700, high byte) and sub-area id (D_00810701, low byte) into a
// 16-bit key and returns the pointer to that area's data block (the
// D_0025D??? tables). Unknown key returns NULL.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the
// switch is lowered as a beq chain; 991202 fills the final branch delay slot
// differently. 2.3.3 reproduces CW's lowering exactly. objdiff 100% vs
// build/expected/func_001F6D60.o.
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern char D_0025D3C0;
extern char D_0025D500;
extern char D_0025D550;
extern char D_0025D5A0;
extern char D_0025D5F0;
extern char D_0025D6C0;
extern char D_0025D760;

char *func_001F6D60(void) {
    int key;

    key = (D_00810700 << 8) + D_00810701;
    switch (key) {
    case 0x100:
        return &D_0025D3C0;
    case 0x700:
        return &D_0025D500;
    case 0x702:
        return &D_0025D550;
    case 0xB00:
        return &D_0025D5A0;
    case 0x1000:
        return &D_0025D5F0;
    case 0x1200:
        return &D_0025D6C0;
    case 0x1300:
        return &D_0025D760;
    default:
        return 0;
    }
}
