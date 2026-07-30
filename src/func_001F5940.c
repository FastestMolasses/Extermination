// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS: spawn/draw helper for a coloured effect.  `kind` (0..9, anything else uses
// the default entry) selects an RGBA quad (col[0..3], 0x80 = full) and a size via a
// 10-entry jump table (jtbl_0026EAF0; slots 3 and 6 both point at the default body), and
// also picks one of three emit modes: 0 = plain func_001F4D40(arg, col, size, size*0.5f);
// 1 = (kind 0) animate the alpha first from the scratchpad counter at 0x70003B68 plus
// (t * 0x12D687) >> 16, wrapped to 0..0x7F, through func_0011DF78 and doubled;
// 2 = (kinds 7/8/9) bracket the emit with depth/priority range changes.

extern int float_to_int(float);
extern float func_0011DF78(float);
extern void func_001F4D40(int, int *, float, float);
extern void func_0021B9A0(int, float, float);

void func_001F5940(unsigned int kind, int arg, int t) {
    int col[4];
    float size;
    float half;

    switch (kind) {
    case 0:
        col[0] = 0;
        col[1] = 0x80;
        col[2] = 0;
        col[3] = 0x7F;
        size = 12.0f;
        kind = 1;
        break;
    case 1:
        col[0] = 0x80;
        col[1] = 0x40;
        col[2] = 0x40;
        col[3] = 0x7F;
        size = 8.0f;
        kind = 0;
        break;
    case 2:
        col[0] = 0x80;
        col[1] = 0x80;
        col[2] = 0x66;
        col[3] = 0x7F;
        size = 8.0f;
        kind = 0;
        break;
    default:
        col[0] = 0x40;
        col[1] = 0x40;
        col[2] = 0x80;
        col[3] = 0x7F;
        size = 3.0f;
        kind = 0;
        break;
    case 4:
        col[0] = 0x80;
        col[1] = 0x40;
        col[2] = 0x40;
        col[3] = 0x7F;
        size = 3.0f;
        kind = 0;
        break;
    case 5:
        col[0] = 0x80;
        col[1] = 0;
        col[2] = 0;
        col[3] = 0x7F;
        size = 30.0f;
        kind = 0;
        break;
    case 7:
        col[0] = 0x20;
        col[1] = 0x80;
        col[2] = 0x20;
        col[3] = 0x7F;
        size = 3.0f;
        kind = 2;
        break;
    case 8:
        col[0] = 0x80;
        col[1] = 0x80;
        col[2] = 0x66;
        col[3] = 0x7F;
        size = 15.0f;
        kind = 2;
        break;
    case 9:
        col[0] = 0x80;
        col[1] = 0x10;
        col[2] = 0x10;
        col[3] = 0x7F;
        size = 3.0f;
        kind = 2;
        break;
    }
    half = 0.5f * size;
    switch (kind) {
    default:
        func_001F4D40(arg, col, size, half);
        break;
    case 1:
        col[3] = (*(volatile int *)0x70003B68 + ((t * 0x12D687) >> 16)) & 0x7F;
        col[3] = float_to_int(2.0f * func_0011DF78((float)col[3] - 64.0f));
        func_001F4D40(arg, col, size, half);
        break;
    case 2:
        func_0021B9A0(2, 0.0f, 100000.0f);
        func_0021B9A0(3, 0.0f, 1000000.0f);
        func_001F4D40(arg, col, size, half);
        /* The shared 0.0f of the restore call is materialised into the SECOND float-arg
           register and copied down (mtc1 zero,$f13 / mov.s $f12,$f13).  Spelling arg2 as
           an assignment to the variable that also supplies arg3 is what makes CW bind the
           constant to $f13 first; a plain `func_0021B9A0(1, 0.0f, 0.0f)` binds it to $f12
           and copies up.  Value is 0.0f either way. */
        { float w = 0.0f; func_0021B9A0(1, (w = 0.0f), w); }
        break;
    }
}
