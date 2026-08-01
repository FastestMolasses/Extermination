// NEARMISS sub_pts_is_not_used  (vram 0xts_is_not_used, 0x188 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// mwcc empty-basic-block / branch-target-padding placement wall. Sizes are equal (98 == 98 instructions) and EVERY register assignment now matches the original exactly (s0=e, s1=i, s2=blk, s3=cont, s4=req, s5=ch, a2=avail, v1=want, a1=off, a0=roff), including the `beqzl` branch-likely with `cont = ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

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
                    if (n < 0) {
                        n = 0;
                    }
                    *(int *)(ch + 0x58) = n;
                }
            } else {
                cont = 0;
            }
            i = (i + 1) % *(int *)(ch + 0x54);
        } while (cont);
    }
    return 0;
}
