// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
//
// SEMANTICS (resolved 2026-06-09 — see docs/FINDINGS.md "ENGINE FRAME
// ANATOMY", step L): DMA CHCR watchdog. For each of D0/D1/D2 CHCR
// (0x10008000 / 0x10009000 / 0x1000A000, byte 0): if the ASP field
// (bits 5:4, the address-stack-pointer depth left by an interrupted
// source-chain transfer) is nonzero, clear it. The dsll32/dsrl32 pairs are
// CW's u8 bitfield extract; the unfolded `andi $v1, $zero, 3; sll` is CW's
// bitfield-store of constant 0 ((0 & 3) << 4, unfolded).
//
// DECOMP ATTEMPT 2026-06-09 — readable C below reaches 97.87%: 46 of 47
// instructions match, including both earlier blocks' filled delay slots and
// all bitfield codegen. The single diff is the THIRD block's `beqz` delay
// slot: CW 2.3.1 fills a branch delay slot only from the TAKEN path (blocks
// 1/2: the next block's `lui $at,0x1001`; block 3's taken path is the
// epilogue, so nop); mwcc 2.3 also fills from the fall-through (the body's
// lui). Documented delay-slot-fill wall; falsified: opt sweep -O2..4 x p/s,
// -opt nospeculate, -sch off, pragmas. Keep the .word form for byte identity.
//
//   typedef struct {            // CHCR byte 0 as CW bitfields
//       unsigned char dir : 1;
//       unsigned char pad : 1;
//       unsigned char mod : 2;
//       unsigned char asp : 2;
//       unsigned char tte : 1;
//       unsigned char tie : 1;
//   } DmaChcrByte;
//   void func_001AB590(void) {    // 97.87%, semantics verified
//       if (((volatile DmaChcrByte *)0x10008000)->asp)
//           ((volatile DmaChcrByte *)0x10008000)->asp = 0;
//       if (((volatile DmaChcrByte *)0x10009000)->asp)
//           ((volatile DmaChcrByte *)0x10009000)->asp = 0;
//       if (((volatile DmaChcrByte *)0x1000A000)->asp)
//           ((volatile DmaChcrByte *)0x1000A000)->asp = 0;
//   }
asm void func_001AB590(void) {
    .word 0x3c011001
    .word 0x90238000
    .word 0x00031ebc
    .word 0x00031fbe
    .word 0x1060000b
    .word 0x3c011001
    .word 0x3c011001
    .word 0x90258000
    .word 0x30030003
    .word 0x00032100
    .word 0x2403ffcf
    .word 0x00a31824
    .word 0x00641825
    .word 0x3c011001
    .word 0xa0238000
    .word 0x3c011001
    .word 0x90239000
    .word 0x00031ebc
    .word 0x00031fbe
    .word 0x1060000b
    .word 0x3c011001
    .word 0x3c011001
    .word 0x90259000
    .word 0x30030003
    .word 0x00032100
    .word 0x2403ffcf
    .word 0x00a31824
    .word 0x00641825
    .word 0x3c011001
    .word 0xa0239000
    .word 0x3c011001
    .word 0x9023a000
    .word 0x00031ebc
    .word 0x00031fbe
    .word 0x1060000a
    .word 0x00000000
    .word 0x3c011001
    .word 0x9025a000
    .word 0x30030003
    .word 0x00032100
    .word 0x2403ffcf
    .word 0x00a31824
    .word 0x00641825
    .word 0x3c011001
    .word 0xa023a000
    .word 0x03e00008
    .word 0x00000000
}
