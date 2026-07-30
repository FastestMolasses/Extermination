// NEARMISS func_001F5940  (vram 0x001F5940, 0x2DC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.92% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 2 instructions, both in the final call of the mode-2 arm: func_0021B9A0(1, 0.0f, 0.0f). Target materializes the shared 0.0f into the SECOND float-arg register and copies down: `mtc1 zero,$f13` ... jal ... delay slot `mov.s $f12,$f13`. mwcc always materializes a CSE'd float constant into the FIRST...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: spawn/draw helper for a coloured effect.  `kind` (0..9, anything
// else uses the default entry) selects an RGBA quad (col[0..3], 0x80 = full)
// and a size, via a 10-entry jump table (jtbl_0026EAF0); table slots 3 and 6
// both point at the default body.  `half` is always size * 0.5f.
// The selected entry also picks one of three emit modes:
//   mode 0 - just func_001F4D40(arg, col, size, half).
//   mode 1 - (kind 0 only) animate the alpha first: derive a phase from the
//            scratchpad counter at 0x70003B68 plus (t * 0x12D687) >> 16, wrap it
//            to 0..0x7F, run it through func_0011DF78 ((phase - 64.0f) -> wave)
//            and store 2.0f * that back into col[3] before emitting.
//   mode 2 - (kinds 7/8/9) bracket the emit with depth/priority range changes:
//            func_0021B9A0(2, 0.0f, 100000.0f) and (3, 0.0f, 1000000.0f) before,
//            func_0021B9A0(1, 0.0f, 0.0f) after.
// NOTE: the `kind` parameter is deliberately reused as the mode variable after
// the first switch (the original keeps both in $a0); `t` is only read on the
// mode-1 path.

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
        func_0021B9A0(1, 0.0f, 0.0f);
        break;
    }
}
