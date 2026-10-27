// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.91% NEARMISS). Fixes read from the original instructions:
// - The live-count clamp is a ternary store (`count = (n < 0) ? 0 : n`), not an
//   if-statement that zeroes n before the store; the if form moved the empty-block
//   padding nop ahead of the store's branch instead of into the else arm.

// Streaming-channel drain step (same subsystem as func_002044F0 / func_00204080).
// `ch` is the channel: +0x08 = sectors-per-block (block size = that << 11),
// +0x50 = pending-request array (24-byte entries), +0x54 = array capacity,
// +0x58 = number of live entries, +0x5C = head index. `req` is the incoming
// request (+0x10 = ring offset, +0x14 = remaining byte count).
//
// Walk the pending entries starting at (head - live) mod capacity. Stop as soon
// as an entry has nothing outstanding, or the incoming request is exhausted. For
// each entry, if the (modular) distance from the request's cursor to the entry's
// cursor is inside the request's span, consume min(entry-remaining, overlap)
// bytes: advance the entry's ring offset modulo the block size and decrement its
// remaining count. When an entry hits zero it is retired — if its 64-bit handle
// at +0x00 is non-negative, notify via func_00204080(&D_00273490) and reset the
// handle pair to -1 and both counters to 0 — and the live count is decremented
// (clamped at 0). If the entry is NOT inside the request's span, stop after this
// iteration. Always returns 0.
extern void func_00204080(void *);
extern int D_00273490;

int sub_pts_is_not_used(char *ch, char *req) {
    char *e;
    int i;
    int blk;
    int cont;
    int n;
    int avail;
    int want;
    int roff;
    int off;
    int take;

    i = (*(int *)(ch + 0x54) + (*(int *)(ch + 0x5C) - *(int *)(ch + 0x58))) % *(int *)(ch + 0x54);
    blk = *(int *)(ch + 0x8) << 0xB;
    cont = 1;
    if (*(int *)(ch + 0x58) > 0) {
        do {
            e = *(char **)(ch + 0x50) + i * 24;
            avail = *(int *)(e + 0x14);
            if (avail == 0) break;
            want = *(int *)(req + 0x14);
            if (want == 0) break;
            off = *(int *)(e + 0x10);
            roff = *(int *)(req + 0x10);
            if (((off + blk) - roff) % blk < want) {
                take = (roff + want) - off;
                avail = (avail < take) ? avail : take;
                *(int *)(e + 0x10) = (off + avail) % blk;
                *(int *)(e + 0x14) -= avail;
                if (*(int *)(e + 0x14) == 0) {
                    if (*(long long *)(e + 0x0) >= 0) {
                        func_00204080(&D_00273490);
                        *(long long *)(e + 0x0) = -1;
                        *(long long *)(e + 0x8) = -1;
                        *(int *)(e + 0x10) = 0;
                        *(int *)(e + 0x14) = 0;
                    }
                    n = *(int *)(ch + 0x58) - 1;
                    *(int *)(ch + 0x58) = (n < 0) ? 0 : n;
                }
            } else {
                cont = 0;
            }
            i = (i + 1) % *(int *)(ch + 0x54);
        } while (cont);
    }
    return 0;
}
