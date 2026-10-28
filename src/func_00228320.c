// NEARMISS func_00228320  (vram 0x00228320, 0x8C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.51% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful.
// Remaining differences: the original copies the 12-byte zero template with a 64-bit
// load plus an integer word load (mwcc 2.3.3 emits a float-register word for every
// struct/array copy spelling tried), and keeps the constant 10 in a register before
// adding 8 (only `total = 10; total += 8; total += s[0]; ...` statements keep it
// unfolded, and those colour the first sum into the wrong register).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Copies the 12-byte all-zero block at D_00266B18 into a 3-int stack temp, then
// sets [0] = (D_0028A4A0->field_0x18 + 0x3FF) >> 10 (bytes rounded up to KiB),
// [1] = 0, [2] = 0. Returns 1 if arg0->field_0x5C >= 10 + 8 + [0] + [1] + [2],
// else 0 (a free-space check).
typedef struct { int v[3]; } Trio;
extern Trio D_00266B18;
extern int *D_0028A4A0;

int func_00228320(char *arg0) {
    Trio s = D_00266B18;
    s.v[0] = (D_0028A4A0[6] + 0x3FF) >> 10;
    s.v[1] = 0;
    s.v[2] = 0;
    if (*(int *)(arg0 + 0x5C) - (s.v[2] + (s.v[1] + (10 + 8 + s.v[0]))) < 0) {
        return 0;
    }
    return 1;
}
