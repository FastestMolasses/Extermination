// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libc (newlib findfp.c): __sfp(reent): runs __sinit (func_00120058) once, then
// finds a free FILE (flags == 0) in the reent's glue list (+0x1D8), growing the
// list by 4 with __sfmoreglue (func_0011FEE8); sets errno (reent +0) to ENOMEM
// and returns NULL when that fails. The found FILE is reset (flags 1, file -1,
// buffers cleared, _data = reent).

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
struct _glue {
    struct _glue *_next;
    int _niobs;
    FILE *_iobs;
};
struct _reent {
    int _errno;
    char pad1[0x34];
    int __sdidinit;          /* 0x38 */
    char pad2[0x1D8 - 0x3C];
    struct _glue __sglue;    /* 0x1D8 */
};
extern void func_00120058(struct _reent *);                     /* __sinit */
extern struct _glue *func_0011FEE8(struct _reent *, int);       /* __sfmoreglue */
#define NDYNAMIC 4
#define ENOMEM 12

FILE *func_0011FF58(struct _reent *d)
{
  FILE *fp;
  int n;
  struct _glue *g;

  if (!d->__sdidinit)
    func_00120058 (d);
  for (g = &d->__sglue;; g = g->_next)
    {
      for (fp = g->_iobs, n = g->_niobs; --n >= 0; fp++)
	if (fp->_flags == 0)
	  goto found;
      if (g->_next == 0 &&
	  (g->_next = func_0011FEE8 (d, NDYNAMIC)) == 0)
	break;
    }
  d->_errno = ENOMEM;
  return 0;

found:
  fp->_flags = 1;		/* reserve this slot; caller sets real flags */
  fp->_p = 0;			/* no current pointer */
  fp->_w = 0;			/* nothing to read or write */
  fp->_r = 0;
  fp->_bf._base = 0;		/* no buffer */
  fp->_bf._size = 0;
  fp->_lbfsize = 0;		/* not line buffered */
  fp->_file = -1;		/* no file */
  /* fp->_cookie = <any>; */	/* caller sets cookie, _read/_write etc */
  fp->_ub._base = 0;		/* no ungetc buffer */
  fp->_ub._size = 0;
  fp->_lb._base = 0;		/* no line buffer */
  fp->_lb._size = 0;
  fp->_data = d;
  return fp;
}
