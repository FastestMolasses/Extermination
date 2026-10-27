// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 75.81% NEARMISS). Fixes read
// from the original instructions:
//  - func_0010FAD0 takes no arguments (its own definition in src/ is (void)); declaring it
//    with arguments gave a1 extra uses and swapped the saved registers of a1 and the
//    D_00279440 block.
//  - func_0010E8A8 is the 9-argument RPC call (cd, fno, mode, send, ssize, recv, rsize,
//    end, endarg): the send and receive buffers are both D_00279440 with size 0x20; the
//    old C had invented an extra leading 0 argument, which shifted every overflow arg.
//  - the a2 range check is unsigned (a2 >= 3 as unsigned), and the result read-back is an
//    if/else-if chain ending in the shared 0xFFFEFFFE return, as in the byte-matched
//    twin func_00110288 (the write-side sibling of this call).
//
// SDK RPC "get" request: ensure the RPC binding is up (func_0010FAD0, failure -> 0xFFFF0000);
// reject sizes >= 3 with 0xFFFEFFFE; store (id a0, size a2) in the shared D_00279440 block
// and issue RPC function 3 on the D_00279640 client with that block as send and receive
// buffer (failure -> 0xFFFEFFFF); then copy the returned value at the start of the block to
// *a1 as a byte (a2 == 0), halfword (a2 == 1) or word (a2 == 2) and return 0.
struct cfg {
    int f0;     // 0x0
    int f4;     // 0x4
};
extern struct cfg D_00279440;
extern int D_00279640[];
extern int func_0010FAD0(void);
extern int func_0010E8A8(void *a, int b, int c, void *d, int e, void *f, int g, int h, int i);

int func_00110198(int a0, unsigned char *a1, unsigned int a2) {
    if (func_0010FAD0() < 0) {
        return 0xFFFF0000;
    }
    if (a2 >= 3) {
        return 0xFFFEFFFE;
    }
    D_00279440.f0 = a0;
    D_00279440.f4 = a2;
    if (func_0010E8A8(D_00279640, 3, 0, &D_00279440, 0x20, &D_00279440, 0x20, 0, 0) < 0) {
        return 0xFFFEFFFF;
    }
    if (a2 == 0) {
        *a1 = *(unsigned char *)&D_00279440.f0;
    } else if (a2 == 1) {
        *(unsigned short *)a1 = *(unsigned short *)&D_00279440.f0;
    } else if (a2 == 2) {
        *(int *)a1 = D_00279440.f0;
    } else {
        return 0xFFFEFFFE;
    }
    return 0;
}
