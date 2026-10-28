// NEARMISS func_00121920  (vram 0x00121920, 0x104 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 81.23% via ee-gcc 2.9-991111-01 (-O2 -fno-strength-reduce) when compiled on its own
// (was 51.82%; matching wave 2026-10-28). Object similarity does not prove semantic
// equivalence. Remaining differences:
// the original was built by a later ee-gcc than 2.9-991111 (its short loops carry the
// Emotion Engine short-loop nop before the back branch and fill the slot); compiled with
// ee-gcc 2.95.3 (tools/eegcc/ee-gcc2.95.3-136 cc1.exe/as.exe via wibo) this exact C is
// instruction-identical except that the original keeps the returned dst pointer in a
// temporary register instead of the first argument register (one register-allocation
// difference, not yet resolved). -fno-strength-reduce reproduces the original's plain
// pointer increments under 2.9 (2.95.3 needs no extra flag).
//
// The function links from the asm body in src/func_00121920.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2 -fno-strength-reduce

// libc: memmove (newlib memmove.c, EE build, same block sizes as memcpy /
// block_copy). A destructive overlap (src below dst inside the source range)
// copies backwards byte by byte; otherwise quadword pairs, doublewords, bytes.
// Returns dst_void.
typedef unsigned int size_t;
typedef int TItype __attribute__((mode(TI)));

#define UNALIGNED(X, Y) \
  (((long)X & (sizeof (TItype) - 1)) | ((long)Y & (sizeof (TItype) - 1)))
#define BIGBLOCKSIZE    (sizeof (TItype) << 1)
#define LITTLEBLOCKSIZE (sizeof (long))
#define TOO_SMALL(LEN)  ((LEN) < BIGBLOCKSIZE)

void *func_00121920(void *dst_void, const char *src, int len) {
  char *dst = dst_void;
  TItype *aligned_dst;
  const TItype *aligned_src;
  long *ldst;
  const long *lsrc;

  if (src < dst && dst < src + len)
    {
      /* Destructive overlap...have to copy backwards */
      src += len;
      dst += len;
      while (len--)
        {
          *--dst = *--src;
        }
    }
  else
    {
      if (!TOO_SMALL(len) && !UNALIGNED (src, dst))
        {
          aligned_dst = (TItype*)dst;
          aligned_src = (TItype*)src;

          while (len >= BIGBLOCKSIZE)
            {
              *aligned_dst++ = *aligned_src++;
              *aligned_dst++ = *aligned_src++;
              len -= BIGBLOCKSIZE;
            }

          ldst = (long*)aligned_dst;
          lsrc = (long*)aligned_src;
          while (len >= LITTLEBLOCKSIZE)
            {
              *ldst++ = *lsrc++;
              len -= LITTLEBLOCKSIZE;
            }

          dst = (char*)ldst;
          src = (char*)lsrc;
        }

      while (len--)
        {
          *dst++ = *src++;
        }
    }

  return dst_void;
}
