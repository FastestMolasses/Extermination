// COMPILER: eegcc
// CFLAGS: -O2
// SDK: newlib _malloc_trim_r(reent_ptr, pad) — releases whole free pages at the top of
// the malloc heap back to the system. top chunk = __malloc_av_[2] (D_00242978+8);
// sbrk_base = D_00242D90; sbrked_mem = D_00242DA8. extra = ((top_size - pad - MINSIZE
// + (pagesz-1)) / pagesz - 1) * pagesz; if a page or more can go, _sbrk_r(-extra) and
// shrink the top chunk header; on sbrk failure re-derive the top size from _sbrk_r(0).
// Returns 1 if memory was released, else 0. This is ee-gcc SDK code (region is the
// newlib mallocr.c cluster with func_00120CE8/_malloc_extend_top): mwcc cannot emit
// the 64-bit long dsubu/daddiu/__udivdi3 idioms (caps ~30-34%).
extern unsigned int func_00122C48(int *, int);   /* _sbrk_r */
extern void func_00121AE8(int *);                /* __malloc_lock */
extern void func_00121AF0(int *);                /* __malloc_unlock */
/* libgcc 64-bit helpers, called explicitly so the relocation carries the
   project symbol name (gcc would otherwise emit __udivdi3/__muldi3): */
extern unsigned long func_00125FA8(unsigned long, unsigned long);  /* __udivdi3 */
extern long func_00125F48(long, long);                             /* __muldi3 */
extern char *D_00242978[];                       /* __malloc_av_ */
extern char *D_00242D90;                         /* __malloc_sbrk_base */
extern int D_00242DA8;                           /* __malloc_sbrked_mem */

#define top (D_00242978[2])

int func_00120408(int *reent_ptr, unsigned int pad) {
    long top_size;
    long extra;
    char *current_brk;
    char *new_brk;
    unsigned long pagesz = 0x1000;

    func_00121AE8(reent_ptr);
    top_size = *(unsigned int *)(top + 4) & ~3;
    /* extra = ((top_size - pad - MINSIZE + (pagesz-1)) / pagesz - 1) * pagesz; */
    extra = func_00125F48(func_00125FA8(top_size - pad - 16 + (pagesz - 1), pagesz) - 1, pagesz);

    if (extra < (long)pagesz) {
        func_00121AF0(reent_ptr);
        return 0;
    } else {
        current_brk = (char *)func_00122C48(reent_ptr, 0);
        if (current_brk != top + top_size) {
            func_00121AF0(reent_ptr);
            return 0;
        } else {
            new_brk = (char *)func_00122C48(reent_ptr, -extra);
            if (new_brk == (char *)-1) {
                current_brk = (char *)func_00122C48(reent_ptr, 0);
                top_size = current_brk - top;
                if (top_size >= 16) {
                    D_00242DA8 = current_brk - D_00242D90;
                    *(int *)(top + 4) = top_size | 1;
                }
                func_00121AF0(reent_ptr);
                return 0;
            } else {
                *(int *)(top + 4) = (top_size - extra) | 1;
                D_00242DA8 -= extra;
                func_00121AF0(reent_ptr);
                return 1;
            }
        }
    }
}
