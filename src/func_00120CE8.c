// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 75.62% NEARMISS).
// Fixes read from the original instructions:
//  - Rewritten in newlib mallocr.c's own shape (malloc_extend_top, dlmalloc 2.6.x,
//    the older version whose second sbrk failure simply returns and whose
//    "brk == old_end" test has no page-alignment check).
//  - Types: top_pad, pagesz, max_sbrked_mem and max_total_mem are 64-bit
//    unsigned long (ee-gcc long = 64 bits); nb, sbrk_size, correction and the
//    chunk sizes are 32-bit INTERNAL_SIZE_T; sbrked_mem is a signed int (read
//    with a sign-extending load before the 64-bit compares).
//  - The old register-order "wall" was the wrong source shape, not the compiler.
//
// SDK: newlib malloc_extend_top(reent_ptr, nb): grows the malloc heap's top chunk.
// sbrk_size = nb + top_pad + MINSIZE (rounded to a page once sbrk_base is set);
// _sbrk_r it; fail if sbrk fails or a foreign sbrk moved the break below the old
// top. If the new space is contiguous, just enlarge top; otherwise record
// sbrk_base (first time) or count the foreign gap in sbrked_mem, align the new
// chunk to 16, sbrk a correction up to the next page boundary, make the new
// space the top chunk, and fence off / free the old top (if it was not the
// initial dummy top). Finally update max_sbrked_mem / max_total_mem.
// av_ = D_00242978 (top = av_[2]), top_pad = D_00242D88, sbrk_base = D_00242D90,
// max_sbrked_mem = D_00242D98, max_total_mem = D_00242DA0, sbrked_mem = D_00242DA8.
struct malloc_chunk {
    unsigned int prev_size;
    unsigned int size;
    struct malloc_chunk *fd;
    struct malloc_chunk *bk;
};
typedef struct malloc_chunk *mchunkptr;
typedef struct malloc_chunk *mbinptr;

extern char *func_00122C48(int *reent_ptr, int incr);          /* _sbrk_r */
extern void func_001200E8(int *reent_ptr, void *mem);          /* _free_r */
extern mbinptr D_00242978[];                                   /* __malloc_av_ */
extern unsigned long D_00242D88;                               /* __malloc_top_pad */
extern char *D_00242D90;                                       /* __malloc_sbrk_base */
extern unsigned long D_00242D98;                               /* __malloc_max_sbrked_mem */
extern unsigned long D_00242DA0;                               /* __malloc_max_total_mem */
extern int D_00242DA8;                                /* __malloc_sbrked_mem */

#define top (D_00242978[2])
#define initial_top ((mchunkptr)D_00242978)
#define chunksize(p) ((p)->size & ~0x3)
#define chunk_at_offset(p, s) ((mchunkptr)(((char *)(p)) + (s)))
#define set_head(p, s) ((p)->size = (s))
#define set_head_size(p, s) ((p)->size = (((p)->size & 0x1) | (s)))
#define chunk2mem(p) ((void *)((char *)(p) + 8))

void func_00120CE8(int *reent_ptr, unsigned int nb) {
    char *brk;
    unsigned int front_misalign;
    unsigned int correction;
    char *new_brk;
    unsigned int top_size;

    mchunkptr old_top = top;
    unsigned int old_top_size = chunksize(old_top);
    char *old_end = (char *)(chunk_at_offset(old_top, old_top_size));

    unsigned int sbrk_size = nb + D_00242D88 + 16;
    unsigned long pagesz = 4096;

    if (D_00242D90 != (char *)(-1))
        sbrk_size = (sbrk_size + (pagesz - 1)) & ~(pagesz - 1);

    brk = (char *)(func_00122C48(reent_ptr, sbrk_size));

    if (brk == (char *)(-1) ||
        (brk < old_end && old_top != initial_top))
        return;

    D_00242DA8 += sbrk_size;

    if (brk == old_end) {
        top_size = sbrk_size + old_top_size;
        set_head(top, top_size | 1);
    } else {
        if (D_00242D90 == (char *)(-1))
            D_00242D90 = brk;
        else
            D_00242DA8 += brk - (char *)old_end;

        front_misalign = (unsigned long)chunk2mem(brk) & 15;
        if (front_misalign > 0) {
            correction = 16 - front_misalign;
            brk += correction;
        } else
            correction = 0;

        correction += pagesz - ((unsigned long)(brk + sbrk_size) & (pagesz - 1));

        new_brk = (char *)(func_00122C48(reent_ptr, correction));
        if (new_brk == (char *)(-1))
            return;

        D_00242DA8 += correction;

        top = (mchunkptr)brk;
        top_size = new_brk - brk + correction;
        set_head(top, top_size | 1);

        if (old_top != initial_top) {
            if (old_top_size < 16) {
                set_head(top, 1);
                return;
            }

            old_top_size = (old_top_size - 3 * 4) & ~15;
            set_head_size(old_top, old_top_size);
            chunk_at_offset(old_top, old_top_size)->size = 4 | 1;
            chunk_at_offset(old_top, old_top_size + 4)->size = 4 | 1;
            if (old_top_size >= 16)
                func_001200E8(reent_ptr, chunk2mem(old_top));
        }
    }

    if ((unsigned long)D_00242DA8 > (unsigned long)D_00242D98)
        D_00242D98 = D_00242DA8;
    if ((unsigned long)(D_00242DA8) > (unsigned long)D_00242DA0)
        D_00242DA0 = D_00242DA8;
}
