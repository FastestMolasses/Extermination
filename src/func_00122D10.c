// COMPILER: eegcc
// CFLAGS: -O2
//
// SDK sprintf: formats into the caller's buffer s with no length limit.
// Builds a stack output-stream record (same record func_00122CA8/func_00122B58
// use): write cursor +0x00 = s, remaining count +0x08 = INT_MAX, mode flags
// +0x0C = 0x208, buffer base +0x10 = s, capacity +0x14 = INT_MAX, and the
// format-handler context +0x54 seeded from the global D_0024295C. Dispatches
// the varargs through func_001236D8 (the lazy-bind formatter driver), then
// NUL-terminates at the final write cursor.
// (__builtin_next_arg(fmt) - 48 == &the a2..t3 vararg register save area,
// i.e. va_start(ap, fmt) for this 2-named-arg EABI frame.)
extern int D_0024295C;
extern int func_001236D8(void *, char *, void *);

typedef struct {
    char *ptr;      /* 0x00 write cursor */
    int unk4;       /* 0x04 */
    int cnt;        /* 0x08 remaining count */
    short flag;     /* 0x0C mode flags */
    short unkE;     /* 0x0E */
    char *base;     /* 0x10 buffer base */
    int len;        /* 0x14 buffer capacity */
    int unk18[15];  /* 0x18..0x53 */
    int handler;    /* 0x54 format handler ctx */
} Stream;

void func_00122D10(char *s, char *fmt, ...)
{
    Stream f;

    f.flag = 0x208;
    f.ptr = s;
    f.cnt = 0x7FFFFFFF;
    f.base = s;
    f.len = 0x7FFFFFFF;
    f.handler = D_0024295C;
    func_001236D8(&f, fmt, (char *)__builtin_next_arg(fmt) - 48);
    *f.ptr = 0;
}
