// NEARMISS func_00211310  (vram 0x00211310, 0xEC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.75% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) (was an asm-word body;
// matching wave 2026-10-28). Twin of the byte-matched func_00211240 (same func_00207E40 call).
// Remaining differences: one integer register: the original builds the 2.0 divisor constant in
// a2, this C in a0 (same instruction otherwise). Expression spellings, float locals, a struct
// parameter, char-pointer access, sdatathreshold 4 and the 991202 / 2.4 compilers tried (17 trials).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Map marker: converts the world position pos (x = pos[0], z = pos[2]) to
// minimap raster units, x = 16 * (1792 + (256 + z) - 8) and
// y = 16 * (1936 + ((164 - x) - 8) / 2) (the same 256+z / 164-x map frame
// that func_00210F30 builds), and draws a 16x16 sprite there with
// func_00207E40 (colour 0x80808080, fixed glyph descriptor).

extern int float_to_int(float f);
extern void func_00207E40(int slot, int x0, int y0, int width, int height, unsigned int rgba, unsigned long long tex0);

void func_00211310(float *pos) {
    int x;
    int y;
    x = float_to_int(16.0f * (1792.0f + ((256.0f + pos[2]) - 8.0f)));
    y = float_to_int(16.0f * (1936.0f + (((164.0f - pos[0]) - 8.0f) / 2.0f)));
    func_00207E40(1, x, y, 0x10, 0x10, 0x80808080, 0x20042E85113221D0ULL);
}
