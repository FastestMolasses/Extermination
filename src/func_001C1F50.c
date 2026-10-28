// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 72.13% NEARMISS).
// Fixes read from the original instructions:
//  - All four tests on the level key are switch statements (mwcc emits the case
//    compares in reverse source order), with cases in ascending source order:
//    the first switch lists the 0xB00..0x1200 group before case 0x1500, the
//    GS-writer switch runs 0xB00..0x1500, and the two trailing single tests
//    (0x1500; 0xF00/0x1200) are switches too, not if statements.
//  - func_001D2830 returns int (its definition in src/).
//
// Level-transition sound/event dispatcher, keyed on the packed 16-bit level id
// (D_00810700<<8 | D_00810701), re-read before each of four switches on it
// (compare chains, not jump tables):
//  1) func_001D2830(0x20,flag)+func_001D2830(0x21,flag)[+more] to arm/disarm a
//     couple of subsystem flags: level 0x1500 arms all four (0x20/21/22/23=1);
//     levels 0x1100/0xF01/0xF00/0xE00/0xD00/0xC00/0xB00/0x1200 arm 0x20/0x21=1 and
//     clear 0x22; anything else clears 0x20 only.
//  2) per-level a 64-bit packed constant is passed to func_001E2260 (a GS-register
//     writer) for levels 0xB00/0xD00/0xE00/0xF00/0xF01/0x1100/0x1200/0x1500; other
//     levels skip the call entirely.
// Then unconditionally func_001E2270(&D_00250F30); then, for level 0x1500,
// func_001E2280 with another 64-bit packed constant; then, for levels
// 0xF00 and 0x1200, func_001D2830(0x25,1).
extern int func_001D2830(int id, int on);
extern void func_001E2260(long long packed);
extern void func_001E2270(void *p);
extern void func_001E2280(long long packed);
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern int D_00250F30;

void func_001C1F50(void) {
    int level;

    level = (D_00810700 << 8) + D_00810701;
    switch (level) {
    case 0xB00:
    case 0xC00:
    case 0xD00:
    case 0xE00:
    case 0xF00:
    case 0xF01:
    case 0x1100:
    case 0x1200:
        func_001D2830(0x20, 1);
        func_001D2830(0x21, 1);
        func_001D2830(0x22, 0);
        break;
    case 0x1500:
        func_001D2830(0x20, 1);
        func_001D2830(0x21, 1);
        func_001D2830(0x22, 1);
        func_001D2830(0x23, 1);
        break;
    default:
        func_001D2830(0x20, 0);
        break;
    }

    level = (D_00810700 << 8) + D_00810701;
    switch (level) {
    case 0xB00:
        func_001E2260(0x21323200 | (0x20069F01LL << 0x20));
        break;
    case 0xD00:
        func_001E2260(0x21323240 | (0x2006D501LL << 0x20));
        break;
    case 0xE00:
        func_001E2260(0x21323440 | (0x2006EA81LL << 0x20));
        break;
    case 0xF00:
        func_001E2260(0x21322F82 | (0x20065901LL << 0x20));
        break;
    case 0xF01:
        func_001E2260(0x21323280 | (0x2006B481LL << 0x20));
        break;
    case 0x1100:
        func_001E2260(0x213230C0 | (0x20066001LL << 0x20));
        break;
    case 0x1200:
        func_001E2260(0x21323300 | (0x2006CC81LL << 0x20));
        break;
    case 0x1500:
        func_001E2260(0x21323700 | (0x20076A81LL << 0x20));
        break;
    }

    func_001E2270(&D_00250F30);

    level = (D_00810700 << 8) + D_00810701;
    switch (level) {
    case 0x1500:
        func_001E2280(0xDD321D00LL | (0x20042B05LL << 0x20));
        break;
    }

    level = (D_00810700 << 8) + D_00810701;
    switch (level) {
    case 0xF00:
    case 0x1200:
        func_001D2830(0x25, 1);
        break;
    }
}
