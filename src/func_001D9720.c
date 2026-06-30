// NEARMISS func_001D9720  (vram 0x001D9720, 0x178 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 57.95% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// u64 GS-register bitfield-struct reconstruction wall (57.95% mwcc 2.3.3, ~75/101 instructions differ). Target writes the DISPLAY register image via a packed 64-bit bitfield union: OR-values held in registers via `daddiu` and combined with register `or` (not `ori`), and literal-0 sub-fields emitted...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_001006D8(unsigned short *, int, int, int);
extern int func_001008C0(void *, int, int, int, long long, long long, long long);
extern int D_00817E20;
extern int D_00817E24;
extern int D_00817E28;
extern int D_00817E2C;
extern unsigned short D_00817E30;
extern unsigned char D_00817E35;
extern unsigned char D_00817E37;
extern unsigned char D_00817E38;
extern unsigned short D_00817E40;
extern int D_00817EC0;

void func_001D9720(void) {
    D_00817E20 = 0;
    D_00817E24 = 0;
    D_00817E28 = 0x11000000;
    D_00817E2C = 0x5000000F;
    D_00817E30 = (unsigned short)((D_00817E30 & ~0x7FFF) | 0xE);
    *(unsigned char *)((char *)&D_00817E30 + 1) =
        (unsigned char)((*(unsigned char *)((char *)&D_00817E30 + 1) & ~0x80) | 0x80);
    D_00817E35 = (unsigned char)((D_00817E35 & ~0x40) | ((0 & 1) << 6));
    D_00817E37 = (unsigned char)((D_00817E37 & ~0xC) | ((0 & 3) << 2));
    D_00817E37 = (unsigned char)((D_00817E37 & ~0xF0) | 0x10);
    D_00817E38 = (unsigned char)((D_00817E38 & ~0xF) | 0xE);
    func_001006D8(&D_00817E40, 0, 0x80, 0x80);
    func_001008C0(&D_00817EC0, 0, 0x7C0, 0x7C0, 0x80, 0, 0);
    D_00817E40 = (unsigned short)((D_00817E40 & ~0x1FF) | 0x12C);
}
