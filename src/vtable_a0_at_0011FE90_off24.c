// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 72.19% NEARMISS
// companion in src/readable/). Fixes read from the original instructions:
//  - written as newlib's own __swrite (stdio.c) over newlib's FILE struct
//    (flags +0xC, file +0xE, reent pointer _data +0x54);
//  - the reent-taking callees return a 64-bit long (newlib's _ssize_t / off_t
//    with ee-gcc's 64-bit long): the original truncates the write result to the
//    int return value, which an int-returning declaration does not do.
//
// libc (newlib stdio): __swrite, the default write hook of a FILE (the +0x24
// function pointer of the stream table at 0x0011FE90). An append-mode stream
// (__SAPP, 0x100) seeks its descriptor to the end (func_00120B38 = _lseek_r,
// SEEK_END); the known-offset flag __SOFF (0x1000) is cleared and the data
// written with func_00124EF8 (_write_r).

typedef struct __sFILE {
    unsigned char *_p;
    int _r;
    int _w;
    short _flags;
    short _file;
    struct { unsigned char *_base; int _size; } _bf;
    int _lbfsize;
    void *_cookie;
    int (*_read)(void *, char *, int);
    int (*_write)(void *, const char *, int);
    long (*_seek)(void *, long, int);
    int (*_close)(void *);
    struct { unsigned char *_base; int _size; } _ub;
    unsigned char *_up;
    int _ur;
    unsigned char _ubuf[3];
    unsigned char _nbuf[1];
    struct { unsigned char *_base; int _size; } _lb;
    int _blksize;
    int _offset;
    void *_data;
} FILE;

extern long func_00120B38(void *ptr, int fd, long pos, int whence);
extern long func_00124EF8(void *ptr, int fd, const void *buf, unsigned int cnt);

int vtable_a0_at_0011FE90_off24(void *cookie, char const *buf, int n) {
    register FILE *fp = (FILE *)cookie;

    if (fp->_flags & 0x100)
        (void)func_00120B38(fp->_data, fp->_file, (long)0, 2);
    fp->_flags &= ~0x1000;
    return func_00124EF8(fp->_data, fp->_file, buf, n);
}
