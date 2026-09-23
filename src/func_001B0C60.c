// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// MATCH NOTE (m3-matching lane, 77.6% -> 100%): the target stores the four
// bytes in the order +B8, +B5, +B7, +B6 with the constant first. mwcc moves a
// non-volatile constant store after the argument stores; declaring the four
// bytes volatile keeps source order. The scratchpad mode byte stays a literal
// on both sides (0x70003B8D is symbolized only for files that name D_70003B8D).
// Parameters are int: the body only sb-stores their low bytes, so it cannot
// decide the width, and every caller (0x00190F7C, 0x00190FD4, 0x001940B0..
// 0x00194220, 0x00196A2C..0x00196CB8) passes constants set with addiu or
// `paddub aN,zero,zero` — mwcc's register move/zero idiom, not byte evidence.
// The matched callers (func_00190F20/00193EB0/00196970) declare (int,int,int);
// int and unsigned char both measure 100% here.
//
// Semantics (target 0x001B0C60): scratchpad mode byte 0x70003B8D = 3, then
// func_001B0C00(4), then D_008106B8 = 1, D_008106B5 = a, D_008106B7 = c,
// D_008106B6 = b.

extern void func_001B0C00(int);
extern volatile char D_008106B8;
extern volatile char D_008106B5;
extern volatile char D_008106B7;
extern volatile char D_008106B6;

void func_001B0C60(int a, int b, int c) {
    *(volatile char *)0x70003B8D = 3;
    func_001B0C00(4);
    D_008106B8 = 1;
    D_008106B5 = a;
    D_008106B7 = c;
    D_008106B6 = b;
}
