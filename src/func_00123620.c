// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libc (newlib vfprintf.c): __sbprintf(fp, fmt, ap): formats an unbuffered
// stream through a 1024-byte stack buffer on a fake FILE (copies _data, flags
// without __SNBF, file, cookie, write hook), fflush (func_0011FD88) after a
// successful vfprintf (func_001236D8), returns EOF if the flush fails, and copies
// __SERR back to fp.

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
typedef char *va_list;
extern int func_001236D8(FILE *fp, const char *fmt, va_list ap);   /* vfprintf */
extern int func_0011FD88(FILE *fp);                                  /* fflush */
#define BUFSIZ 1024
#define __SNBF 0x0002
#define __SERR 0x0040
#define EOF (-1)

int func_00123620(FILE *fp, const char *fmt, va_list ap)
{
	int ret;
	FILE fake;
	unsigned char buf[BUFSIZ];

	/* copy the important variables */
	fake._data = fp->_data;
	fake._flags = fp->_flags & ~__SNBF;
	fake._file = fp->_file;
	fake._cookie = fp->_cookie;
	fake._write = fp->_write;

	/* set up the buffer */
	fake._bf._base = fake._p = buf;
	fake._bf._size = fake._w = sizeof(buf);
	fake._lbfsize = 0;	/* not actually used, but Just In Case */

	/* do the work, then copy any error status */
	ret = func_001236D8(&fake, fmt, ap);
	if (ret >= 0 && func_0011FD88(&fake))
		ret = EOF;
	if (fake._flags & __SERR)
		fp->_flags |= __SERR;
	return (ret);
}
