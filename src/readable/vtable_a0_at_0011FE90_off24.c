// NEARMISS vtable_a0_at_0011FE90_off24  (vram 0x00122DE8, 0x80 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 72.19% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc register allocation of the reent / descriptor reloads (not iterated).
//
// The function links from the asm body in src/vtable_a0_at_0011FE90_off24.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// libc (newlib stdio): __swrite, the default write hook of a FILE (the +0x24
// function pointer of the stream table at 0x0011FE90). An append-mode stream
// (flag 0x100) seeks its descriptor (+0xE) to the end (func_00120B38,
// lseek_r(reent +0x54, fd, 0, SEEK_END)); the known-offset flag 0x1000 is
// cleared and the data written with func_00124EF8 (write_r).
extern long func_00120B38(void *reent, int fd, long off, int whence);
extern int func_00124EF8(void *reent, int fd, const char *buf, int n);

int vtable_a0_at_0011FE90_off24(char *fp, const char *buf, int n) {
    if (*(unsigned short *)(fp + 0xC) & 0x100) {
        func_00120B38(*(void **)(fp + 0x54), *(short *)(fp + 0xE), 0, 2);
    }
    *(unsigned short *)(fp + 0xC) &= ~0x1000;
    return func_00124EF8(*(void **)(fp + 0x54), *(short *)(fp + 0xE), buf, n);
}
