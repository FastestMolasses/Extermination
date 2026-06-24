// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Status/type lookup table selector. Packs the two global state bytes
// D_00810700 (hi) and D_00810701 (lo) into a 16-bit selector and returns the
// pointer to the matching parameter/descriptor block (one of eight 0x25xxxx
// tables) for that combined state; returns NULL for any unrecognised value.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the
// switch case-chain dispatch matches under 2.3.3 (tests cases in descending
// order with the idiom-13/beql delay-slot fills CW emits). 991202 schedules
// the dispatch differently. Verified objdiff 100.0 vs
// build/expected/func_001F6760.o.
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern char D_0025CF10[];
extern char D_0025CF90[];
extern char D_0025CFE0[];
extern char D_0025D030[];
extern char D_0025D080[];
extern char D_0025D1F0[];
extern char D_0025D270[];
extern char D_0025D340[];

char *func_001F6760(void) {
    int v;
    v = (D_00810700 << 8) + D_00810701;
    switch (v) {
    case 0x0:    return D_0025CF10;
    case 0x1:    return D_0025CF90;
    case 0x2:    return D_0025CFE0;
    case 0x100:  return D_0025D030;
    case 0x200:  return D_0025D080;
    case 0xE00:  return D_0025D1F0;
    case 0x1100: return D_0025D340;
    case 0x1301: return D_0025D270;
    }
    return 0;
}
