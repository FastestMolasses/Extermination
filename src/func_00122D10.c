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
// (va_start is ee-gcc's EABI form, as in func_00122CA8: the cursor starts at the
// first unnamed register slot, 8 - 2 named = 6 slots of 8 bytes below
// __builtin_next_arg(fmt).)

typedef char *va_list;
#define va_start(ap, last) \
    ((ap) = (char *)__builtin_next_arg(last) - \
     (__builtin_args_info(2) < 8 ? (8 - __builtin_args_info(2)) * 8 : 0))
#define va_end(ap) ((void)0)
extern int D_0024295C;
extern int func_001236D8(void *, char *, va_list);

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
    va_list ap;

    f.flag = 0x208;
    f.ptr = s;
    f.cnt = 0x7FFFFFFF;
    f.base = s;
    f.len = 0x7FFFFFFF;
    f.handler = D_0024295C;
    va_start(ap, fmt);
    func_001236D8(&f, fmt, ap);
    va_end(ap);
    *f.ptr = 0;
}
