// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libc (newlib mallocr.c): _calloc_r(reent, n, size): _malloc_r (func_00120F40)
// of n * size bytes, then dlmalloc's MALLOC_ZERO: chunks up to 36 usable bytes are
// cleared with unrolled word stores, larger ones with memset (func_00121A28).
// Returns NULL when the allocation fails.

typedef unsigned int size_t;
typedef size_t INTERNAL_SIZE_T;
struct _reent;
struct malloc_chunk { INTERNAL_SIZE_T prev_size; INTERNAL_SIZE_T size; };
typedef struct malloc_chunk *mchunkptr;
#define SIZE_SZ (sizeof(INTERNAL_SIZE_T))
#define mem2chunk(mem) ((mchunkptr)((char *)(mem) - 2 * SIZE_SZ))
#define PREV_INUSE 0x1
#define IS_MMAPPED 0x2
#define SIZE_BITS (PREV_INUSE | IS_MMAPPED)
#define chunksize(p) ((p)->size & ~(SIZE_BITS))
extern void *func_00120F40(struct _reent *ptr, size_t bytes);   /* _malloc_r */
extern void *func_00121A28(void *s, int c, size_t n);           /* memset */
#define MALLOC_ZERO(charp, nbytes)                                            \
do {                                                                          \
  INTERNAL_SIZE_T mzsz = (nbytes);                                            \
  if(mzsz <= 9*sizeof(mzsz)) {                                                \
    INTERNAL_SIZE_T* mz = (INTERNAL_SIZE_T*) (charp);                         \
    if(mzsz >= 5*sizeof(mzsz)) {     *mz++ = 0;                               \
                                     *mz++ = 0;                               \
      if(mzsz >= 7*sizeof(mzsz)) {   *mz++ = 0;                               \
                                     *mz++ = 0;                               \
        if(mzsz >= 9*sizeof(mzsz)) { *mz++ = 0;                               \
                                     *mz++ = 0; }}}                           \
                                     *mz++ = 0;                               \
                                     *mz++ = 0;                               \
                                     *mz   = 0;                               \
  } else  func_00121A28((charp), 0, mzsz);                                    \
} while(0)

void *func_0011E878(struct _reent *ptr, size_t n, size_t elem_size)
{
  mchunkptr p;
  INTERNAL_SIZE_T csz;
  INTERNAL_SIZE_T sz = n * elem_size;
  void *mem;

  mem = func_00120F40 (ptr, sz);
  if (mem == 0)
    return 0;
  else
    {
      p = mem2chunk (mem);
      csz = chunksize (p);
      MALLOC_ZERO (mem, csz - SIZE_SZ);
      return mem;
    }
}
