// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 89.93% NEARMISS).
// Fixes read from the original instructions:
//  - The flag word is a struct field at +0x174, and its address is taken into
//    a pointer once (`&D_00275670->flags`); the store goes through that pointer.
//  - The old word is read through the pointer, and the "was set" test reads the
//    field again through the struct after it; that spelling gives the original
//    register colouring (one load, address kept in its own register).
//
// Sets (set != 0) or clears bit (bit - 32) of the flag word D_00275670 + 0x174
// and returns whether it was set before.

struct S { char pad[0x174]; int flags; };
extern struct S *D_00275670;

int func_001E0C80(int bit, int set) {
    int n = bit - 0x20;
    int *p = &D_00275670->flags;
    int mask = 1 << n;
    int v = *p;
    int was = D_00275670->flags & mask;
    if (set) {
        v |= mask;
    } else {
        v &= ~mask;
    }
    *p = v;
    return was != 0;
}
