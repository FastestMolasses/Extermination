// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 57.95% NEARMISS).
// Fixes read from the original instructions:
//  - func_001006D8 takes SIX arguments (a pointer and five 16-bit values): the
//    call passes (&A+D block, 0, 0x80, 0x80, 0, 2); the old 4-argument extern
//    dropped the last two.
//  - func_001008C0 (libgraph clear-packet builder) takes eleven arguments:
//    (p, ztest 0, x 0x7C0, y 0x7C0, w 0x80, h 0x80, r/g/b 0x80, a 0, z 0).
//  - The GIFtag at +0x10 is a struct of 64-bit bitfields written field by
//    field (NLOOP, EOP, PRE, FLG, NREG, REGS0), not hand-written byte masks.
//  - The final 9-bit merge at the A+D block is a 64-bit bitfield store on the
//    global itself (absolute address, not through a pointer register).
//
// Builds the static packet at D_00817E20: two zero words, the VIF words
// 0x11000000 / 0x5000000F, a GIFtag (NLOOP 14, EOP, PRE off, PACKED, one
// A+D register), fills the A+D register block at +0x20 with func_001006D8
// (0x80 x 0x80), writes a default clear packet at +0xA0 with func_001008C0,
// then sets the low 9 bits of the first A+D word to 0x12C.

extern void func_001006D8(char *, short, short, short, short, short);
extern int func_001008C0(void *, short, short, short, short, short,
                         unsigned char, unsigned char, unsigned char, unsigned char, unsigned int);

typedef struct {
    unsigned long long NLOOP : 15;
    unsigned long long EOP : 1;
    unsigned long long pad16 : 30;
    unsigned long long PRE : 1;
    unsigned long long PRIM : 11;
    unsigned long long FLG : 2;
    unsigned long long NREG : 4;
    unsigned long long REGS0 : 4;
    unsigned long long REGS1 : 60;
} GifTag;

typedef struct {
    unsigned long long X : 9;
    unsigned long long rest : 55;
} AdReg;

typedef struct {
    int vif[4];
    GifTag tag;
    AdReg ad;
    char ad_rest[0x78];
    char clear[0x60];
} Pkt;

extern Pkt D_00817E20;

void func_001D9720(void) {
    D_00817E20.vif[0] = 0;
    D_00817E20.vif[1] = 0;
    D_00817E20.vif[2] = 0x11000000;
    D_00817E20.vif[3] = 0x5000000F;
    D_00817E20.tag.NLOOP = 0xE;
    D_00817E20.tag.EOP = 1;
    D_00817E20.tag.PRE = 0;
    D_00817E20.tag.FLG = 0;
    D_00817E20.tag.NREG = 1;
    D_00817E20.tag.REGS0 = 0xE;
    func_001006D8((char *)&D_00817E20.ad, 0, 0x80, 0x80, 0, 2);
    func_001008C0(D_00817E20.clear, 0, 0x7C0, 0x7C0, 0x80, 0x80, 0x80, 0x80, 0x80, 0, 0);
    D_00817E20.ad.X = 0x12C;
}
