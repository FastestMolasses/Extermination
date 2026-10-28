// NEARMISS block_copy  (vram 0x00121870, 0xB0 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 86.70% via ee-gcc 2.9-991111-01 (-O2 -fno-strength-reduce) when compiled on its own
// (was 54.86%; matching wave 2026-10-28). Object similarity does not prove semantic
// equivalence. Remaining differences:
// the original was built by a later ee-gcc than 2.9-991111 (its short loops carry the
// Emotion Engine short-loop nop before the back branch and fill the slot); compiled with
// ee-gcc 2.95.3 (tools/eegcc/ee-gcc2.95.3-136 cc1.exe/as.exe via wibo) this exact C is
// instruction-identical except that the original keeps the returned dst pointer in a
// temporary register instead of the first argument register (one register-allocation
// difference, not yet resolved). -fno-strength-reduce reproduces the original's plain
// pointer increments under 2.9 (2.95.3 needs no extra flag).
//
// The function links from the asm body in src/block_copy.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2 -fno-strength-reduce

// libc: memcpy (newlib memcpy.c, EE build: the block type is a 128-bit quadword,
// so BIGBLOCKSIZE = 32 and the copy is two quadwords per step; LITTLEBLOCKSIZE is
// sizeof (long) = 8 with ee-gcc's 64-bit long). The length is an int; the
// TOO_SMALL test compares it against an unsigned sizeof, so it is an unsigned
// compare. Doublewords follow while 8 or more bytes remain, then bytes.
// Returns dst0.
typedef unsigned int size_t;
typedef int TItype __attribute__((mode(TI)));

#define UNALIGNED(X, Y) \
  (((long)X & (sizeof (TItype) - 1)) | ((long)Y & (sizeof (TItype) - 1)))
#define BIGBLOCKSIZE    (sizeof (TItype) << 1)
#define LITTLEBLOCKSIZE (sizeof (long))
#define TOO_SMALL(LEN)  ((LEN) < BIGBLOCKSIZE)

void *block_copy(void *dst0, const char *src, int len) {
  char *dst = dst0;
  TItype *aligned_dst;
  const TItype *aligned_src;
  long *ldst;
  const long *lsrc;

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
    *dst++ = *src++;

  return dst0;
}
