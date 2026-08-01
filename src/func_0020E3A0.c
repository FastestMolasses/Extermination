// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Maps a small signed item/slot code to the ASCII glyph used to draw it.
// The dispatch is a 24-entry jump table over code+2 (valid range -2..21);
// anything outside the range, and the eight in-range codes that have no glyph
// of their own (-1, 13, 14, 15, 17..20), fall back to '/' (0x2F).
//
//   -2 -> '0'   0 -> '2'   1 -> '3'   2 -> '4'   3 -> '5'   4 -> '6'
//    5 -> '1'   6 -> '7'   7 -> '8'   8 -> '9'   9 -> ':'  10 -> ';'
//   11 -> '<'  12 -> '='  16 -> '@'  21 -> 'm'  else '/'
//
// The in-range-but-default cases are written out explicitly (they are what the
// original's 24-entry table encodes) and the case labels are kept in the
// original's emission order — CodeWarrior lays switch bodies out in source
// order, and both details are load-bearing for the match.
int func_0020E3A0(int code) {
    int ch;

    switch (code) {
    case -1:
    case 13:
    case 14:
    case 15:
    case 17:
    case 18:
    case 19:
    case 20:
    default: ch = 0x2F; break;
    case -2: ch = 0x30; break;
    case 16: ch = 0x40; break;
    case 0:  ch = 0x32; break;
    case 1:  ch = 0x33; break;
    case 2:  ch = 0x34; break;
    case 3:  ch = 0x35; break;
    case 4:  ch = 0x36; break;
    case 5:  ch = 0x31; break;
    case 6:  ch = 0x37; break;
    case 7:  ch = 0x38; break;
    case 8:  ch = 0x39; break;
    case 9:  ch = 0x3A; break;
    case 10: ch = 0x3B; break;
    case 11: ch = 0x3C; break;
    case 12: ch = 0x3D; break;
    case 21: ch = 0x6D; break;
    }
    return ch;
}
