// NEARMISS func_001DAFA0  (vram 0x001DAFA0, 0xA0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.88% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Float-store body (bulk) fully matches. Loop-preamble residual: register coloring + mwcc reloads the base symbol where target does a reg-move (a0->t0) + one delay-slot fill difference. Regalloc/scheduling permutation, not the clean-store nop.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Zeroes fields +8/+0xC of a 0x20x0x20 grid of 0x10-byte entries (row stride
// 0x200) at D_00818000, then writes 8 float constants into the trailing block
// at base+0x4000..+0x402C. Base pointer kept live across the loop so the float
// stores use 0x40xx(base) immediate offsets (matches target exactly).
// NEARMISS: o233 91.88%. The entire 8-store float block matches byte-for-byte.
// Residuals are confined to the loop preamble: register coloring (a1/a2/t0 vs
// a3/a4/a2), one extra symbol reload where target does a reg-move (a0->t0), and
// one delay-slot fill (target fills inner-ptr init, mwcc emits nop). Regalloc /
// scheduling permutation -> permuter territory.
extern unsigned char D_00818000[];

void func_001DAFA0(void) {
    unsigned char *base = D_00818000;
    unsigned char *row = base;
    int i;
    int j;

    i = 0;
    do {
        unsigned char *p = row;
        j = 0;
        do {
            j += 1;
            *(int *)(p + 0xC) = 0;
            *(int *)(p + 8) = 0;
            p += 0x10;
        } while (j < 0x20);
        i += 1;
        row += 0x200;
    } while (i < 0x20);
    *(int *)(base + 0x4000) = 0x3F7DB22D;
    *(int *)(base + 0x4004) = 0x3CB43958;
    *(int *)(base + 0x400C) = 0x3ECCCCCD;
    *(int *)(base + 0x4008) = 0x3D4CCCCD;
    *(int *)(base + 0x4020) = 0x43990CCD;
    *(int *)(base + 0x4024) = 0x43380000;
    *(int *)(base + 0x4028) = 0x43E61CE0;
    *(int *)(base + 0x402C) = 0x3F800000;
}
