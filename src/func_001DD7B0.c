// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 64.55% NEARMISS).
// Fixes read from the original instructions:
//  - func_001006D8 takes SIX arguments (a pointer and five 16-bit values): the
//    call passes (D_0081C070, 0, 0x100, 0x100, 0, 2). The old 4-argument extern
//    dropped the last two, which looked like register-move noise.
//  - D_0081C060 is a GIFtag bitfield struct (64-bit fields: NLOOP, EOP, PRE,
//    FLG, NREG, REGS0); the field stores replace the hand-written byte
//    mask-and-or sequence (they give the unfolded inserts of zero and the
//    64-bit OR constants).
//  - The record pointer (globalctx + 0x24F0) is computed first.
//  - D_0081C070 is typed as a 9-bit bitfield halfword array so the final merge
//    of D_0027568C >> 13 is a bitfield store addressed directly.
//
// GS / DMA register-block initializer. Zeroes D_0081C050/54, sets D_0081C058 =
// 0x11000000 and D_0081C05C = 0x50000009 (VIF words), then fills the A+D
// GIFtag at D_0081C060 (NLOOP 8, EOP 1, PRE 0, FLG 0 = packed, NREG 1, REGS0
// 0xE = A+D). Calls func_001006D8 to fill the A+D register at D_0081C070 from
// 0x100 x 0x100, then replaces its low 9 bits with (D_0027568C >> 13). Finally
// zeroes a 0x20-byte command/state record at globalctx(D_00275670)+0x24F0 and
// sets its +0x20 word to 1.
extern int func_001006D8(char *, short, short, short, short, short);
extern char *D_00275670;
extern int D_0027568C;

typedef struct {
    unsigned long long NLOOP : 15;
    unsigned long long EOP : 1;
    unsigned long long pad16 : 16;
    unsigned long long id : 14;
    unsigned long long PRE : 1;
    unsigned long long PRIM : 11;
    unsigned long long FLG : 2;
    unsigned long long NREG : 4;
    unsigned long long REGS0 : 4;
    unsigned long long REGS1 : 4;
    unsigned long long REGS2 : 4;
    unsigned long long REGS3 : 4;
    unsigned long long REGS4 : 4;
    unsigned long long REGS5 : 4;
    unsigned long long REGS6 : 4;
    unsigned long long REGS7 : 4;
    unsigned long long REGS8 : 4;
    unsigned long long REGS9 : 4;
    unsigned long long REGS10 : 4;
    unsigned long long REGS11 : 4;
    unsigned long long REGS12 : 4;
    unsigned long long REGS13 : 4;
    unsigned long long REGS14 : 4;
    unsigned long long REGS15 : 4;
} GifTag;

typedef struct {
    unsigned short lo : 9;
    unsigned short hi : 7;
} Half9;

extern int D_0081C050[3];
extern int D_0081C054[3];
extern int D_0081C058[3];
extern int D_0081C05C[3];
extern GifTag D_0081C060;
extern Half9 D_0081C070[8];

void func_001DD7B0(void) {
    char *p;

    p = D_00275670 + 0x24F0;
    D_0081C050[0] = 0;
    D_0081C054[0] = 0;
    D_0081C058[0] = 0x11000000;
    D_0081C05C[0] = 0x50000009;
    D_0081C060.NLOOP = 8;
    D_0081C060.EOP = 1;
    D_0081C060.PRE = 0;
    D_0081C060.FLG = 0;
    D_0081C060.NREG = 1;
    D_0081C060.REGS0 = 0xE;
    func_001006D8((char *)D_0081C070, 0, 0x100, 0x100, 0, 2);
    D_0081C070[0].lo = D_0027568C >> 13;
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
