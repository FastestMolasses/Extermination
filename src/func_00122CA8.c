// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libc (newlib sprintf.c, older form): _sprintf_r(reent, str, fmt, ...): a
// string FILE on the stack (flags __SWR | __SSTR, buffer = str, size INT_MAX,
// _data = reent) through vfprintf (func_001236D8), then NUL-terminates.

/* ee-gcc's EABI varargs (gcc 2.9 va-mips.h, no <stdarg.h> in this toolchain
   setup): the unnamed register arguments are spilled as 8-byte slots just
   below __builtin_next_arg(last); va_list is a plain cursor that starts at the
   first unnamed slot (8 argument registers minus the named ones). */
typedef char *va_list;
#define va_start(ap, last) \
    ((ap) = (char *)__builtin_next_arg(last) - \
     (__builtin_args_info(2) < 8 ? (8 - __builtin_args_info(2)) * 8 : 0))
#define va_end(ap) ((void)0)
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
struct _reent;
extern int func_001236D8(FILE *fp, const char *fmt, va_list ap);

int func_00122CA8(struct _reent *ptr, char *str, const char *fmt, ...)
{
  int ret;
  va_list ap;
  FILE f;

  f._flags = 0x0008 | 0x0200;
  f._bf._base = f._p = (unsigned char *) str;
  f._bf._size = f._w = 0x7fffffff;
  f._data = ptr;
  va_start (ap, fmt);
  ret = func_001236D8 (&f, fmt, ap);
  va_end (ap);
  *f._p = 0;
  return ret;
}
