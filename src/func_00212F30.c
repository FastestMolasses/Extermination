// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: draws one of ten fixed HUD/UI sprites.  `id & 0xFF` selects a
// hard-coded parameter set (a 10-entry jump table; any other value draws
// nothing) and hands it to func_00207E40, the textured-sprite emitter:
//   func_00207E40(layer, x, y, w, h, ctx, tex0)
//     layer = 1 for every entry here (selects the display list at
//             D_00275670[+0x10 + layer*4]).
//     x, y  = GS screen position in 1/16-pixel units (func_00207E40 adds w*16
//             to x and h*8 to y to form the second XY of the sprite).
//     w, h  = sprite size in texels; they always equal the TW/TH fields that
//             func_00207E40 decodes out of `tex0` (bits 26-29 and 30-33).
//     ctx   = this function's second argument, forwarded unchanged.
//     tex0  = a 64-bit GS TEX0 register value (TBP0/TBW/PSM in the low word,
//             TW/TH/TCC/TFX/CBP/CPSM/CSA/CLD in the high word).
// The four 0x7380/0x7EA0/256x128 entries (ids 1..4) are the same slot drawn
// from four different texture pages, so they are most likely the frames of one
// animated panel.

extern void func_00207E40(int, int, int, int, int, int, long long);

void func_00212F30(int id, int ctx)
{
    switch (id & 0xFF) {
    case 0:
        func_00207E40(1, 0x7C80, 0x8060, 0x40, 0x40, ctx, 0x20046F0599322310LL);
        break;
    case 1:
        func_00207E40(1, 0x7380, 0x7EA0, 0x100, 0x80, ctx, 0x20046F85E1321E00LL);
        break;
    case 2:
        func_00207E40(1, 0x7380, 0x7EA0, 0x100, 0x80, ctx, 0x20047605E1321E40LL);
        break;
    case 3:
        func_00207E40(1, 0x7380, 0x7EA0, 0x100, 0x80, ctx, 0x20047685E1321F00LL);
        break;
    case 4:
        func_00207E40(1, 0x7380, 0x7EA0, 0x100, 0x80, ctx, 0x20047805E1321F40LL);
        break;
    case 5:
        func_00207E40(1, 0x7C80, 0x7FA0, 0x40, 0x20, ctx, 0x20046D8559322350LL);
        break;
    case 6:
        func_00207E40(1, 0x7C80, 0x7EE0, 0x80, 0x40, ctx, 0x200478859D322280LL);
        break;
    case 7:
        func_00207E40(1, 0x8080, 0x7F20, 0x40, 0x20, ctx, 0x20047A0559322360LL);
        break;
    case 8:
        func_00207E40(1, 0x8080, 0x7EE0, 0x40, 0x20, ctx, 0x20047A8559322370LL);
        break;
    case 9:
        func_00207E40(1, 0x8080, 0x7EE0, 0x40, 0x20, ctx, 0x20047C0559322328LL);
        break;
    }
}
