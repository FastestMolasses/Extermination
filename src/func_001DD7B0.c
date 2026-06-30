// NEARMISS func_001DD7B0  (vram 0x001DD7B0, 0x188 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 64.55% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Whole-function instruction scheduling + register coloring over a packed GS-bitfield read-modify-write (target uses daddiu+or 64-bit OR-constants vs ori, unfolded (0&mask)<<shift, pointer-base materialization for byte+1 access, paddub register moves). Not the clean-store nop -> regalloc/scheduling...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// GS / DMA register-block initializer. Zeroes D_0081C050/54, sets D_0081C058 =
// 0x11000000 and D_0081C05C = 0x50000009 (GIFtag-like words), then packs a set
// of bitfields into the GS register block at D_0081C060 (short field[0:14]=8,
// byte+1 bit7=1, byte+5 (0x65) bits cleared, byte+7 (0x67) nibble fields=0x10,
// byte+8 (0x68) low nibble=0xE). Calls func_001006D8 to fill D_0081C070 with
// 0x100/0x100, then patches its low 9 bits with (D_0027568C >> 13). Finally
// zeroes a 0x20-byte command/state record at globalctx(D_00275670)+0x24F0 and
// sets its +0x20 word to 1.
//
// NEARMISS: logic fully recovered. Residual at 64.6% (mwcc 2.3.3) is whole-
// function instruction scheduling + register coloring over the packed-bitfield
// read-modify-write sequence, NOT the clean-store nop. The target materializes
// the OR-constants as 64-bit (daddiu rX,zero,8 ; or  vs mwcc's ori), keeps the
// unfolded (0 & mask) << shift inserts as live shifts, materializes a base
// pointer for the D_0081C060+1 byte access (vs lui/%lo per access), and emits
// paddub reg,reg,zero register moves -- all regalloc/scheduling-class artifacts
// requiring the exact original bitfield struct + a coloring permutation. The
// -sdatathreshold 8 + sized-array decls correctly split gp-rel (D_00275670/
// D_0027568C) from absolute (0x81Cxxx) addressing, matching the target.
extern void func_001006D8(unsigned short *p, int a, int b, int c);
extern int D_00275670;
extern int D_0027568C;
extern int D_0081C050[3];
extern int D_0081C054[3];
extern int D_0081C058[3];
extern int D_0081C05C[3];
extern unsigned short D_0081C060[8];
extern unsigned char D_0081C065[16];
extern unsigned char D_0081C067[16];
extern unsigned char D_0081C068[16];
extern unsigned short D_0081C070[8];

void func_001DD7B0(void) {
    char *p;

    D_0081C050[0] = 0;
    D_0081C054[0] = 0;
    D_0081C058[0] = 0x11000000;
    D_0081C05C[0] = 0x50000009;
    p = (char *)(D_00275670 + 0x24F0);
    D_0081C060[0] = (D_0081C060[0] & ~0x7FFF) | 8;
    *((unsigned char *)D_0081C060 + 1) = (*((unsigned char *)D_0081C060 + 1) & ~0x80) | 0x80;
    D_0081C065[0] = (D_0081C065[0] & ~0x40) | 0;
    D_0081C067[0] = (D_0081C067[0] & ~0xC) | 0;
    D_0081C067[0] = (D_0081C067[0] & ~0xF0) | 0x10;
    D_0081C068[0] = (D_0081C068[0] & ~0xF) | 0xE;
    func_001006D8(D_0081C070, 0, 0x100, 0x100);
    D_0081C070[0] = (D_0081C070[0] & ~0x1FF) | ((D_0027568C >> 13) & 0x1FF);
    *(int *)(p + 0xC) = 0;
    *(int *)(p + 8) = 0;
    *(int *)(p + 4) = 0;
    *(int *)(p + 0) = 0;
    *(int *)(p + 0x1C) = 0;
    *(int *)(p + 0x18) = 0;
    *(int *)(p + 0x14) = 0;
    *(int *)(p + 0x10) = 0;
    *(int *)(p + 0x20) = 1;
}
