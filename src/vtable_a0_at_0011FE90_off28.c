// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libc (newlib stdio.c): __sseek, the default seek hook: _lseek_r
// (func_00120B38); -1 clears __SOFF, otherwise sets it and records the offset
// (+0x50). Returns the 64-bit result.

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

long vtable_a0_at_0011FE90_off28(void *cookie, long offset, int whence)
{
    register FILE *fp = (FILE *)cookie;
    register long ret;

    ret = func_00120B38(fp->_data, fp->_file, offset, whence);
    if (ret == -1L)
        fp->_flags &= ~0x1000;
    else {
        fp->_flags |= 0x1000;
        fp->_offset = ret;
    }
    return ret;
}
