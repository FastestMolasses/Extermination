// COMPILER: eegcc
// CFLAGS: -O2
//
// Doug Lea malloc (dlmalloc 2.6.x) `fREe` from the Sony PS2 SDK libc, with the
// arena/lock handle threaded through as the first parameter. NOT CodeWarrior
// game code -- this is SDK/lowmem territory, built with ee-gcc 2.9-991111-01
// (sd/ld 64-bit register saves, `daddu rd,rs,zero` moves, unfilled jal;nop).
//
// Layout recovered from the code (SIZE_SZ = 4, MALLOC_ALIGNMENT = 8):
//   chunk:  +0 prev_size, +4 size (bit0 = PREV_INUSE, bit1 = IS_MMAPPED),
//           +8 fd, +0xC bk
//   D_00242978 = av_[], the bin array. bin_at(i) == (char *)av_ + 8*i;
//                av_+4 = binblocks, av_+8 = bin_at(0)->fd == `top`.
//   D_00242980 = av_ + 8 == bin_at(1) == `last_remainder`.
//   D_00242D80 = trim_threshold (64-bit), D_00242D88 = top_pad.
//   func_00121AE8 / func_00121AF0 = MALLOC_LOCK / MALLOC_UNLOCK (the unlock is
//                a tail call, hence `j func_00121AF0` in both exits).
//   func_00120408 = malloc_trim(pool, top_pad).
//
// Flow: free(0) is a no-op; otherwise coalesce the chunk backward (if the
// previous chunk is free) and forward (if the next chunk is free), keeping
// `last_remainder` as last_remainder when it is one of the neighbours (islr);
// if the merged chunk abuts `top` it is absorbed into top and the arena is
// trimmed once the top block passes trim_threshold; otherwise the chunk is
// frontlink'ed into its bin -- smallbin (sz < 512, index sz>>3, mark the
// binblock bit) or the size-ordered large bin found by bin_index().
//
// Matching notes (ee-gcc lane): the ~PREV_INUSE / ~SIZE_BITS masks must be
// written as the unsigned literals 0xFFFFFFFE / 0xFFFFFFFC so gcc materialises
// them with lui/ori instead of `addiu -2`; av_ needs THREE separate pointer
// locals (av/avs/avl) so gcc shares one `lui %hi` but rematerialises the
// `addiu %lo` per block, exactly as the target does; and trim_threshold must be
// read into a local before the two top-merge stores so the 64-bit `ld` is
// scheduled ahead of them (that is what puts set_head into the branch delay
// slot). objdiff 100.0 -- 199/199 instructions byte-identical.

extern void func_00121AE8(int pool);
extern void func_00121AF0(int pool);
extern void func_00120408(int pool, int top_pad);

extern char D_00242978[];        /* av_[]                                    */
extern char D_00242980[];        /* bin_at(1) == last_remainder              */
extern unsigned long D_00242D80; /* trim_threshold                           */
extern int D_00242D88;           /* top_pad                                  */

void func_001200E8(int pool, char *mem)
{
    char *av;
    char *avs;
    char *avl;
    unsigned long tt;
    char *p;
    char *next;
    char *bck;
    char *fwd;
    int hd;
    unsigned int sz;
    unsigned int nextsz;
    unsigned int prevsz;
    int idx;
    int islr;

    if (mem == 0)
        return;

    func_00121AE8(pool);

    av = D_00242978;
    p = mem - 8;                                    /* mem2chunk(mem)        */
    hd = *(int *)(p + 4);
    sz = hd & 0xFFFFFFFE;                           /* ~PREV_INUSE           */
    next = p + sz;
    nextsz = *(int *)(next + 4) & 0xFFFFFFFC;       /* chunksize(next)       */

    if (next == *(char **)(av + 8)) {               /* merge with top        */
        sz += nextsz;
        if (!(hd & 1)) {                            /* consolidate backward  */
            prevsz = *(unsigned int *)p;
            p -= prevsz;
            sz += prevsz;
            bck = *(char **)(p + 0xC);              /* unlink(p, bck, fwd)   */
            fwd = *(char **)(p + 8);
            *(char **)(fwd + 0xC) = bck;
            *(char **)(bck + 8) = fwd;
        }
        tt = D_00242D80;
        *(char **)(av + 8) = p;                     /* top = p               */
        *(int *)(p + 4) = sz | 1;                   /* set_head(p, sz|1)     */
        if ((unsigned long)sz >= tt)
            func_00120408(pool, D_00242D88);        /* malloc_trim(top_pad)  */
        func_00121AF0(pool);
        return;
    }

    *(int *)(next + 4) = nextsz;                    /* clear next's inuse bit*/
    islr = 0;

    if (!(hd & 1)) {                                /* consolidate backward  */
        prevsz = *(unsigned int *)p;
        p -= prevsz;
        sz += prevsz;
        if (*(char **)(p + 8) == av + 8) {          /* keep as last_remainder*/
            islr = 1;
        } else {
            bck = *(char **)(p + 0xC);              /* unlink(p, bck, fwd)   */
            fwd = *(char **)(p + 8);
            *(char **)(fwd + 0xC) = bck;
            *(char **)(bck + 8) = fwd;
        }
    }

    if (!(*(int *)(next + nextsz + 4) & 1)) {       /* consolidate forward   */
        sz += nextsz;
        if (islr == 0 && *(char **)(next + 8) == D_00242980) {
            islr = 1;                               /* link_last_remainder(p)*/
            *(char **)(D_00242980 + 0xC) = p;
            *(char **)(D_00242980 + 8) = p;
            *(char **)(p + 0xC) = D_00242980;
            *(char **)(p + 8) = D_00242980;
        } else {
            bck = *(char **)(next + 0xC);           /* unlink(next,bck,fwd)  */
            fwd = *(char **)(next + 8);
            *(char **)(fwd + 0xC) = bck;
            *(char **)(bck + 8) = fwd;
        }
    }

    *(int *)(p + 4) = sz | 1;                       /* set_head(p, sz|1)     */
    *(unsigned int *)(p + sz) = sz;                 /* set_foot(p, sz)       */

    if (islr == 0) {                                /* frontlink(p, sz, ...) */
        if (sz < 0x200) {
            idx = sz >> 3;                          /* smallbin_index(sz)    */
            avs = D_00242978;
            bck = (idx << 3) + avs;                 /* bin_at(idx)           */
            fwd = *(char **)(bck + 8);
            *(char **)(p + 0xC) = bck;
            *(char **)(p + 8) = fwd;
            *(char **)(bck + 8) = p;
            *(char **)(fwd + 0xC) = p;
            /* mark_binblock(idx): binblocks |= 1 << (idx / BINBLOCKWIDTH) */
            *(int *)(avs + 4) |= (int)((unsigned long)1 << (idx / 4));
        } else {
            if ((sz >> 9) == 0)                     /* bin_index(sz)         */
                idx = sz >> 3;
            else if ((sz >> 9) <= 4)
                idx = 0x38 + (sz >> 6);
            else if ((sz >> 9) <= 0x14)
                idx = 0x5B + (sz >> 9);
            else if ((sz >> 9) <= 0x54)
                idx = 0x6E + (sz >> 12);
            else if ((sz >> 9) <= 0x154)
                idx = 0x77 + (sz >> 15);
            else if ((sz >> 9) <= 0x554)
                idx = 0x7C + (sz >> 18);
            else
                idx = 0x7E;

            avl = D_00242978;
            bck = (idx << 3) + avl;                 /* bin_at(idx)           */
            fwd = *(char **)(bck + 8);
            if (fwd == bck) {
                *(int *)(avl + 4) |= (int)((unsigned long)1 << (idx / 4));
            } else {
                /* keep the bin ordered by decreasing chunk size */
                while (fwd != bck && sz < (*(unsigned int *)(fwd + 4) & 0xFFFFFFFC))
                    fwd = *(char **)(fwd + 8);
                bck = *(char **)(fwd + 0xC);
            }
            *(char **)(p + 0xC) = bck;
            *(char **)(p + 8) = fwd;
            *(char **)(bck + 8) = p;
            *(char **)(fwd + 0xC) = p;
        }
    }

    func_00121AF0(pool);
}
