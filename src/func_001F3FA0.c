// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 72.60% NEARMISS).
// Fixes read from the original instructions:
// - The 0x4E1340 "symbol" is not a variable: it is the constant offset of an
//   element's flag field inside the big block at D_0028F700 (element array at
//   +0x4E12C0, 0x90-byte stride, flag at element offset 0x80). Writing the
//   access as a struct member gives the per-iteration large-offset store the
//   original has; the old pointer-plus-symbol spelling made mwcc hoist it.
// - The element loop indexes the array (for i in 0..0x7F) instead of carrying
//   a byte pointer.
// - The clear counter is initialised before the element pointer is taken
//   (pointer declared first, assigned after), which fixes the schedule and
//   register choice.

// Initializes 0x80 elements of a 0x90-byte-stride array at offset 0x4E12C0
// of the block D_0028F700: clears the first 9 quadwords (0x90 bytes) of each
// element, then sets the 16-bit flag at element offset 0x80 to 1. Finally
// zeroes the gp-relative globals D_00275C40 and D_00275C44.
typedef int s128 __attribute__((mode(TI)));
typedef struct {
    s128 q[8];
    short flag;
    char pad[14];
} Elem;
typedef struct {
    char pad[0x4E12C0];
    Elem elems[0x80];
} Big;
extern int D_00275C40;
extern int D_00275C44;
extern Big D_0028F700;

void func_001F3FA0(void) {
    int i;
    for (i = 0; i < 0x80; i++) {
        s128 *p;
        int n = 9;
        p = (s128 *)&D_0028F700.elems[i];
        do {
            *p = 0;
            n -= 1;
            p += 1;
        } while (n != 0);
        D_0028F700.elems[i].flag = 1;
    }
    D_00275C40 = 0;
    D_00275C44 = 0;
}
