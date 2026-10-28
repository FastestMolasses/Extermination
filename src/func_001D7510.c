// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 57.73% NEARMISS).
// Fixes read from the original instructions:
// - arg1 and arg2 are int parameters (the original sign-extends each from 32
//   bits before shifting it into the final dword), not longs cast back to int.
// - The slot is &D_00275670[idx] (char **) with the cursor read as slot[4],
//   and the block pointer kept in its own local before the cursor advance, as
//   in the byte-matched twins (func_001D6E60 / func_001D71A0).
// - The chip select is written `== 0 ? 0 : 0x70000` (the zero is the default
//   value and the 0x70000 is conditionally moved in), not `!= 0 ? 0x70000 : 0`.
// - The final dword is arg1 << 35 OR'd with ((chip >> 8) | constant) OR'd with
//   arg2 << 34, in that grouping.
//
// Per-channel GIFtag/DMA-tag builder: takes the channel's write pointer
// (D_00275670 slot idx, table +0x10), stamps its DMA tag (byte +3 = 0x10,
// word +4 = 0, half +0 = 4 qwords), advances the pointer by 0x50, zeroes the
// next quadword and writes VIF word 0x50000003 at +0x1C, then the A+D GIFtag
// 0x10000000_00008002 with register list 0xE, a zero dword with register 0x3F,
// and a dword (arg1 << 35) | (arg2 << 34) | (chip >> 8) | 0x2_24020000 for
// register 6, where chip = 0x70000 when the word at table +0x9C is non-zero.
typedef int s128 __attribute__((mode(TI)));
extern char **D_00275670;

void func_001D7510(int idx, int arg1, int arg2) {
    char **slot;
    char *e;
    int chip;

    chip = (*(int *)((char *)D_00275670 + 0x9C) == 0) ? 0 : 0x70000;
    slot = (char **)(&D_00275670[idx]);
    *(char *)(slot[4] + 3) = 0x10;
    *(int *)(slot[4] + 4) = 0;
    *(short *)(slot[4] + 0) = 4;
    e = slot[4];
    slot[4] = e + 0x50;
    *(s128 *)(e + 0x10) = 0;
    *(int *)(e + 0x1C) = 0x50000003;
    *(long *)(e + 0x20) = 0x8002L | ((long)0x10000000 << 32);
    *(long *)(e + 0x28) = 0xE;
    *(long *)(e + 0x30) = 0;
    *(long *)(e + 0x38) = 0x3F;
    *(long *)(e + 0x40) = ((long)arg1 << 35) |
                          (((long)(chip >> 8) | (0x24020000L | ((long)2 << 32))) |
                           ((long)arg2 << 34));
    *(long *)(e + 0x48) = 6;
}
