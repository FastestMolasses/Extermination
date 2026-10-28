// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 84.68% NEARMISS).
// Fixes read from the original instructions:
//  - func_001006D8 takes SIX arguments (a pointer and five 16-bit values; its
//    body sign-extends a1..a3, t0 and t1 from 16 bits): the call passes
//    (q, 0, 1 << arg2, 1 << arg3, 0, 2). The old 4-argument extern dropped the
//    last two (0 and 2), which looked like a constant-register "permutation".
//  - The slot is &D_00275670[arg0] (char **) with the cursor read as slot[4],
//    and the body pointer (cursor + 0x10) is taken before the cursor advance,
//    as in the byte-matched twins (func_001D6580 / func_001D64A0).
//  - Locals declared in the order q, p, e, slot (sets the saved registers).
//  - The final 9-bit merge into the first halfword at q is a bitfield store
//    (low 9 bits = arg1 >> 13), not a hand-written mask-and-or.
//
// DMA/GIF packet builder. Claims a 0xB0-byte block from the channel cursor
// D_00275670[arg0][4]: stamps the DMAtag (byte +3 = 0x10, word +4 = 0, half
// +0 = 0xA qwords), advances the cursor by 0xB0, then writes the body: two zero
// words at +0x10/+0x14, VIF words 0x11000000 (+0x18) and 0x50000009 (+0x1C),
// the A+D GIFtag 0x10000000_00008008 (+0x20) and register list 0xE (+0x28).
// func_001006D8 fills the A+D registers at body + 0x20 from the texture
// dimensions (1 << arg2, 1 << arg3); the low 9 bits of the first halfword are
// then replaced with arg1 >> 13. Returns the body pointer (cursor + 0x10).
extern char **D_00275670;
extern int func_001006D8(char *, short, short, short, short, short);

typedef struct {
    unsigned short lo : 9;
    unsigned short hi : 7;
} Half9;

char *func_001D6E60(int arg0, int arg1, int arg2, int arg3) {
    char *q;
    char *p;
    char *e;
    char **slot;

    slot = (char **)(&D_00275670[arg0]);
    *(char *)(slot[4] + 3) = 0x10;
    *(int *)(slot[4] + 4) = 0;
    *(short *)(slot[4] + 0) = 0xA;
    e = slot[4];
    p = e + 0x10;
    slot[4] = e + 0xB0;
    *(int *)(e + 0x10) = 0;
    *(int *)(e + 0x14) = 0;
    *(int *)(e + 0x18) = 0x11000000;
    *(int *)(e + 0x1C) = 0x50000009;
    *(long long *)(e + 0x20) = 0x8008 | ((long long)0x10000000 << 0x20);
    *(long long *)(e + 0x28) = 0xE;
    q = p + 0x20;
    func_001006D8(q, 0, 1 << arg2, 1 << arg3, 0, 2);
    ((Half9 *)q)->lo = arg1 >> 0xD;
    return p;
}
