// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// DMA CHCR watchdog. For each of D0/D1/D2 CHCR (0x10008000 / 0x10009000 /
// 0x1000A000, byte 0): if the ASP field (bits 5:4, the address-stack-pointer
// depth left by an interrupted source-chain transfer) is nonzero, clear it.
// The dsll32/dsrl32 pairs are CW's u8 bitfield extract; the andi/sll is CW's
// bitfield-store of constant 0. (Semantics resolved 2026-06-09; see
// docs/FINDINGS.md "ENGINE FRAME ANATOMY", step L.)
// 991202 reaches 97.87% (the 3rd block's beqz delay slot is the clean-store
// idiom-13 nop CW leaves but mwcc 991202 fills); mwcc 2.3.3 byte-matches.
typedef struct {            // CHCR byte 0 as CW bitfields
    unsigned char dir : 1;
    unsigned char pad : 1;
    unsigned char mod : 2;
    unsigned char asp : 2;
    unsigned char tte : 1;
    unsigned char tie : 1;
} DmaChcrByte;

void func_001AB590(void) {
    if (((volatile DmaChcrByte *)0x10008000)->asp)
        ((volatile DmaChcrByte *)0x10008000)->asp = 0;
    if (((volatile DmaChcrByte *)0x10009000)->asp)
        ((volatile DmaChcrByte *)0x10009000)->asp = 0;
    if (((volatile DmaChcrByte *)0x1000A000)->asp)
        ((volatile DmaChcrByte *)0x1000A000)->asp = 0;
}
