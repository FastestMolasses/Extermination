// COMPILER: eegcc
// CFLAGS: -O2

/* SDK: newlib mallocr.c _malloc_r (Doug Lea malloc 2.6.4), PS2 MALLOC_ALIGNMENT=16
   build (MAX_SMALLBIN=63 double-step bin walk). av_ = D_00242978 (__malloc_av_);
   last_remainder = bin_at(1) = D_00242980. __malloc_lock/__malloc_unlock =
   func_00121AE8/func_00121AF0; malloc_extend_top = func_00120CE8. */

struct malloc_chunk {
    unsigned int prev_size;
    unsigned int size;
    struct malloc_chunk *fd;
    struct malloc_chunk *bk;
};
typedef struct malloc_chunk *mchunkptr;
typedef struct malloc_chunk *mbinptr;

extern void func_00121AE8(int *);               /* __malloc_lock */
extern void func_00121AF0(int *);               /* __malloc_unlock */
extern void func_00120CE8(int *, unsigned int); /* malloc_extend_top */
extern mbinptr D_00242978[];                    /* __malloc_av_ */
extern mbinptr D_00242980[];                    /* __malloc_av_ + 8: bin_at(1), the
                                                   last_remainder bin (splat names the
                                                   mid-array address separately) */

#define SIZE_SZ           4
#define MALLOC_ALIGN_MASK 15
#define MINSIZE           0x10
#define MAX_SMALLBIN      63
#define MAX_SMALLBIN_SIZE 512
#define SMALLBIN_WIDTH    8
#define BINBLOCKWIDTH     4

#define PREV_INUSE 0x1
#define IS_MMAPPED 0x2
#define SIZE_BITS  (PREV_INUSE | IS_MMAPPED)

#define av_ D_00242978

#define bin_at(i)   ((mbinptr)((char *)&(av_[2 * (i) + 2]) - 2 * SIZE_SZ))
#define next_bin(b) ((mbinptr)((char *)(b) + 2 * sizeof(mbinptr)))
#define prev_bin(b) ((mbinptr)((char *)(b) - 2 * sizeof(mbinptr)))
#define first(b)    ((b)->fd)
#define last(b)     ((b)->bk)

#define top            (bin_at(0)->fd)
#define last_remainder ((mbinptr)D_00242980)
#define binblocks      (bin_at(0)->size)

#define chunk2mem(p)          ((void *)((char *)(p) + 2 * SIZE_SZ))
#define chunksize(p)          ((p)->size & ~(SIZE_BITS))
#define chunk_at_offset(p, s) ((mchunkptr)(((char *)(p)) + (s)))
#define set_head(p, s)        ((p)->size = (s))
#define set_foot(p, s)        (((mchunkptr)((char *)(p) + (s)))->prev_size = (s))
#define set_inuse_bit_at_offset(p, s) \
    (((mchunkptr)(((char *)(p)) + (s)))->size |= PREV_INUSE)

#define request2size(req) \
    (((long)((req) + (SIZE_SZ + MALLOC_ALIGN_MASK)) < \
      (long)(MINSIZE + MALLOC_ALIGN_MASK)) ? MINSIZE : \
     (((req) + (SIZE_SZ + MALLOC_ALIGN_MASK)) & ~(MALLOC_ALIGN_MASK)))

#define long_sub_size_t(x, y) ((x) < (y) ? -((long)((y) - (x))) : ((x) - (y)))

#define is_small_request(nb) ((nb) < MAX_SMALLBIN_SIZE - SMALLBIN_WIDTH)

#define smallbin_index(sz) (((unsigned int)(sz)) >> 3)

#define bin_index(sz) \
    (((((unsigned int)(sz)) >> 9) ==    0) ?        (((unsigned int)(sz)) >>  3) : \
     ((((unsigned int)(sz)) >> 9) <=    4) ?  (56 + (((unsigned int)(sz)) >>  6)) : \
     ((((unsigned int)(sz)) >> 9) <=   20) ?  (91 + (((unsigned int)(sz)) >>  9)) : \
     ((((unsigned int)(sz)) >> 9) <=   84) ? (110 + (((unsigned int)(sz)) >> 12)) : \
     ((((unsigned int)(sz)) >> 9) <=  340) ? (119 + (((unsigned int)(sz)) >> 15)) : \
     ((((unsigned int)(sz)) >> 9) <= 1364) ? (124 + (((unsigned int)(sz)) >> 18)) : \
     126)

#define idx2binblock(ii)  ((unsigned long)1 << ((ii) / BINBLOCKWIDTH))
#define mark_binblock(ii) (binblocks |= idx2binblock(ii))

#define unlink(P, BK, FD) \
{                         \
    BK = P->bk;           \
    FD = P->fd;           \
    FD->bk = BK;          \
    BK->fd = FD;          \
}

#define frontlink(P, S, IDX, BK, FD)                       \
{                                                          \
    if (S < MAX_SMALLBIN_SIZE)                             \
    {                                                      \
        char *abase = (char *)last_remainder - 8;          \
        IDX = smallbin_index(S);                           \
        ((mbinptr)abase)->size |= idx2binblock(IDX);       \
        BK = (mbinptr)(abase + ((unsigned int)(IDX) << 3)); \
        FD = BK->fd;                                       \
        P->bk = BK;                                        \
        P->fd = FD;                                        \
        FD->bk = BK->fd = P;                               \
    }                                                      \
    else                                                   \
    {                                                      \
        IDX = bin_index(S);                                \
        BK = bin_at(IDX);                                  \
        FD = BK->fd;                                       \
        if (FD == BK) mark_binblock(IDX);                  \
        else                                               \
        {                                                  \
            while (FD != BK && S < chunksize(FD)) FD = FD->fd; \
            BK = FD->bk;                                   \
        }                                                  \
        P->bk = BK;                                        \
        P->fd = FD;                                        \
        FD->bk = BK->fd = P;                               \
    }                                                      \
}

#define link_last_remainder(P)                    \
{                                                 \
    last_remainder->fd = last_remainder->bk = P;  \
    P->fd = P->bk = last_remainder;               \
}

#define clear_last_remainder \
    (last_remainder->fd = last_remainder->bk = last_remainder)

void *func_00120F40(int *reent_ptr, unsigned int bytes)
{
    mchunkptr victim;            /* inspected/selected chunk */
    unsigned int victim_size;    /* its size */
    int idx;                     /* index for bin traversal */
    mbinptr bin;                 /* associated bin */
    mchunkptr remainder;         /* remainder from a split */
    long remainder_size;         /* its size */
    int remainder_index;         /* its bin index */
    unsigned long block;         /* block traverser bit */
    int startidx;                /* first bin of a traversed block */
    mchunkptr fwd;               /* misc temp for linking */
    mchunkptr bck;               /* misc temp for linking */
    mbinptr q;                   /* misc temp */

    unsigned int nb = request2size(bytes);  /* padded request size */

    func_00121AE8(reent_ptr);    /* MALLOC_LOCK */

    /* Check for exact match in a bin */

    if (is_small_request(nb))    /* Faster version for small requests */
    {
        idx = smallbin_index(nb);

        /* No traversal or size check necessary for small bins.  */

        q = bin_at(idx);
        victim = last(q);

        if (victim != q)
        {
            victim_size = chunksize(victim);
            unlink(victim, bck, fwd);
            set_inuse_bit_at_offset(victim, victim_size);
            func_00121AF0(reent_ptr);
            return chunk2mem(victim);
        }

        idx += 2;  /* Set for bin scan below. We've already scanned 2 bins. */
    }
    else
    {
        idx = bin_index(nb);
        bin = bin_at(idx);

        for (victim = last(bin); victim != bin; victim = victim->bk)
        {
            victim_size = chunksize(victim);
            remainder_size = long_sub_size_t(victim_size, nb);

            if (remainder_size >= (long)MINSIZE)  /* too big */
            {
                --idx;  /* adjust to rescan below after checking last remainder */
                break;
            }
            else if (remainder_size >= 0)  /* exact fit, but not exhausted */
            {
                unlink(victim, bck, fwd);
                set_inuse_bit_at_offset(victim, victim_size);
                func_00121AF0(reent_ptr);
                return chunk2mem(victim);
            }
        }

        ++idx;
    }

    /* Try to use the last split-off remainder */

    if ((victim = last_remainder->fd) != last_remainder)
    {
        victim_size = chunksize(victim);
        remainder_size = long_sub_size_t(victim_size, nb);

        if (remainder_size >= (long)MINSIZE)  /* re-split */
        {
            remainder = chunk_at_offset(victim, nb);
            set_head(victim, nb | PREV_INUSE);
            link_last_remainder(remainder);
            set_head(remainder, remainder_size | PREV_INUSE);
            set_foot(remainder, remainder_size);
            func_00121AF0(reent_ptr);
            return chunk2mem(victim);
        }

        clear_last_remainder;

        if (remainder_size >= 0)  /* exhaust */
        {
            set_inuse_bit_at_offset(victim, victim_size);
            func_00121AF0(reent_ptr);
            return chunk2mem(victim);
        }

        /* Else place in bin */

        frontlink(victim, victim_size, remainder_index, bck, fwd);
    }

    /* If there are any possibly nonempty big-enough blocks,
       search for best fitting chunk by scanning bins in blockwidth units. */

    if ((block = idx2binblock(idx)) <= binblocks)
    {
        /* Get to the first marked block */

        if ((block & binblocks) == 0)
        {
            /* force to an even block boundary */
            idx = (idx & ~(BINBLOCKWIDTH - 1)) + BINBLOCKWIDTH;
            block <<= 1;
            while ((block & binblocks) == 0)
            {
                idx += BINBLOCKWIDTH;
                block <<= 1;
            }
        }

        /* For each possibly nonempty block ... */
        for (;;)
        {
            startidx = idx;  /* (track incomplete blocks) */
            q = bin = bin_at(idx);

            /* For each bin in this block ... */
            do
            {
                /* Find and use first big enough chunk ... */
                for (victim = last(bin); victim != bin; victim = victim->bk)
                {
                    victim_size = chunksize(victim);
                    remainder_size = long_sub_size_t(victim_size, nb);

                    if (remainder_size >= (long)MINSIZE)  /* split */
                    {
                        remainder = chunk_at_offset(victim, nb);
                        set_head(victim, nb | PREV_INUSE);
                        unlink(victim, bck, fwd);
                        link_last_remainder(remainder);
                        set_head(remainder, remainder_size | PREV_INUSE);
                        set_foot(remainder, remainder_size);
                        func_00121AF0(reent_ptr);
                        return chunk2mem(victim);
                    }
                    else if (remainder_size >= 0)  /* take */
                    {
                        set_inuse_bit_at_offset(victim, victim_size);
                        unlink(victim, bck, fwd);
                        func_00121AF0(reent_ptr);
                        return chunk2mem(victim);
                    }
                }

                bin = next_bin(bin);

                if (idx < MAX_SMALLBIN)
                {
                    bin = next_bin(bin);
                    ++idx;
                }

            } while ((++idx & (BINBLOCKWIDTH - 1)) != 0);

            /* Clear out the block bit. */

            do  /* Possibly backtrack to try to clear a partial block */
            {
                if ((startidx & (BINBLOCKWIDTH - 1)) == 0)
                {
                    binblocks &= ~block;
                    break;
                }
                --startidx;
                q = prev_bin(q);
            } while (first(q) == q);

            /* Get to the next possibly nonempty block */

            if ((block <<= 1) <= binblocks && (block != 0))
            {
                while ((block & binblocks) == 0)
                {
                    idx += BINBLOCKWIDTH;
                    block <<= 1;
                }
            }
            else
                break;
        }
    }

    /* Try to use top chunk */

    /* Require that there be a remainder, ensuring top always exists  */
    remainder_size = long_sub_size_t(chunksize(top), nb);
    if (chunksize(top) < nb || remainder_size < (long)MINSIZE)
    {
        /* Try to extend */
        func_00120CE8(reent_ptr, nb);  /* malloc_extend_top */
        remainder_size = long_sub_size_t(chunksize(top), nb);
        if (chunksize(top) < nb || remainder_size < (long)MINSIZE)
        {
            func_00121AF0(reent_ptr);
            return 0;  /* propagate failure */
        }
    }

    victim = top;
    set_head(victim, nb | PREV_INUSE);
    top = chunk_at_offset(victim, nb);
    set_head(top, remainder_size | PREV_INUSE);
    func_00121AF0(reent_ptr);
    return chunk2mem(victim);
}
