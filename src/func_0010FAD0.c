// COMPILER: eegcc
// CFLAGS: -O2
//
// libmc/libcdvd-family SDK init retry loop. While the D_00241C98 latch is negative,
// repeatedly calls func_0010E6F8(&D_00279640, 0x80000006, 0); a negative result aborts
// with -1. When the +0x24 field of the request block becomes nonzero the latch is
// cleared and func_0010E8A8(block, 0xFF, 0,0,0, &D_00279440, 4, 0, 0) is issued (9th
// argument passed on the stack); on failure it returns 0xFFFEFFFF, on success it copies
// the 4-byte result D_00279440 -> D_00279668 and returns 0. Between polls it burns a
// 0x100000-iteration hand-rolled delay loop.
//
// The unaligned 4-byte copy MUST be __builtin_memcpy to get ee-gcc's lwl/lwr/swl/swr
// pair. NOTE for future measurement: the delay loop's `lui $v0,0x10` / `addiu $v0,$v0,-1`
// pair is symbolized by splat as %hi(D_FFFFF)/%lo(D_FFFFF); the canonical expected
// object is built from build/.asmnorm/, where tools/decomp/build.py's _ASM_FIXUPS undo
// that invented symbol. Assembling build/asm/matchings/main/code/func_0010FAD0.s
// directly leaves an unreproducible relocation and shows a false 99.84%.
extern int func_0010E6F8(void *p, unsigned int flags, int x);
extern int func_0010E8A8(void *p, int a1, int a2, int a3, int a4, void *a5, int a6, int a7, int a8);

extern int D_00241C98;
extern unsigned char D_00279640[];
extern unsigned char D_00279440[];
extern unsigned char D_00279668[];

int func_0010FAD0(void) {
    void *s0;
    int r;

    if (D_00241C98 < 0) {
        for (;;) {
            s0 = (void *)D_00279640;
            if (func_0010E6F8(s0, 0x80000006u, 0) < 0)
                return -1;
            r = *(int *)((char *)s0 + 0x24);
            if (r != 0) {
                D_00241C98 = 0;
                if (func_0010E8A8(s0, 0xFF, 0, 0, 0, (void *)D_00279440, 4, 0, 0) < 0)
                    return 0xFFFEFFFF;
                __builtin_memcpy((void *)D_00279668, (void *)D_00279440, 4);
                return 0;
            }
            {
                int i = 0x00100000;
                do {
                    i--;
                } while (i != -1);
            }
        }
    }
    return 0;
}
