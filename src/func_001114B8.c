// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 85.20% NEARMISS).
// Fixes read from the original instructions:
// - Not a branch-likely wall: written in the byte-matched siblings' shape (a record
//   pointer to D_00279A80 kept in a local, an early `return 0` on a negative result,
//   then the 0xC field read through the pointer), ee-gcc emits the original's
//   forward branch-likely with the field load in its delay slot.
//
// SDK: configure the request record D_00279A80 (type 0x12) and send it via
// func_0010E8A8 (record is both send and receive buffer, 0x80 bytes each);
// on success return the reply word at +0xC, else 0.

struct cfg {
    int f0;     // 0x0 request type
    int f4;     // 0x4
    int f8;     // 0x8
    int fC;     // 0xC reply value
};
extern struct cfg D_00279A80;
extern char D_00279700[];
extern int func_0010E8A8(void *a, int b, int c, void *d, int e, void *f, int g, int h, int i);

int func_001114B8(void) {
    struct cfg *d = &D_00279A80;
    d->f0 = 0x12;
    if (func_0010E8A8(D_00279700, 1, 0, d, 0x80, d, 0x80, 0, 0) < 0) {
        return 0;
    }
    return d->fC;
}
