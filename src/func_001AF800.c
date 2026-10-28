// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 59.06% NEARMISS).
// Fixes read from the original instructions:
// - each record is zeroed with 13 quadword stores (a 128-bit element type, as in
//   the twin func_001AF890), not 52 word stores; the "scheduling padding" was
//   just the quadword store loop's normal shape
// - the owner is a record struct (count byte at +9, flag byte at +0xC, slot
//   pointer array at +0x110) walked with an indexed for-loop
// - the push is the twin's `*--D_00275BD0 = blk` and the count update `+=`
//
// Releases every record owned by r: for each of its r->count slots, zeroes the
// 13-quadword (0xD0-byte) record, pushes it back on the free stack D_00275BD0
// and clears the slot; then adds the released count to D_00275BCC and clears
// the count and the flag byte at +0xC.

typedef unsigned int u128 __attribute__((mode(TI)));
extern char **D_00275BD0;
extern short D_00275BCC;

typedef struct {
    char pad0[9];
    unsigned char count;
    char pad1[2];
    unsigned char flag;
    char pad2[0x110 - 0xD];
    char *slot[1];
} Rec;

void func_001AF800(Rec *r) {
    int i;
    u128 *p;
    int n;

    for (i = 0; i < r->count; i++) {
        p = (u128 *)r->slot[i];
        n = 13;
        do {
            *p = 0;
            n--;
            p++;
        } while (n != 0);
        *--D_00275BD0 = r->slot[i];
        r->slot[i] = 0;
    }
    D_00275BCC += r->count;
    r->count = 0;
    r->flag = 0;
}
