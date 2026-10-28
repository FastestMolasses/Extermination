// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libc (newlib stdio.c): __sread, the default read hook of a FILE: _read_r
// (func_00122BE8, 64-bit result truncated); a non-negative count advances
// _offset (+0x50), an error clears __SOFF (0x1000).

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
extern long func_00122BE8(void *ptr, int fd, void *buf, unsigned int cnt);

int vtable_a0_at_0011FE90_off20(void *cookie, char *buf, int n)
{
    register FILE *fp = (FILE *)cookie;
    register int ret;

    ret = func_00122BE8(fp->_data, fp->_file, buf, n);
    if (ret >= 0)
        fp->_offset += ret;
    else
        fp->_flags &= ~0x1000;
    return ret;
}
