// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libc (newlib printf.c): _printf_r(reent, fmt, ...): _vfprintf_r
// (sub__0000000000000000Inf) on the reent's stdout (+8) with the varargs.

/* ee-gcc's EABI varargs (gcc 2.9 va-mips.h, no <stdarg.h> in this toolchain
   setup): the unnamed register arguments are spilled as 8-byte slots just
   below __builtin_next_arg(last); va_list is a plain cursor that starts at the
   first unnamed slot (8 argument registers minus the named ones). */
typedef char *va_list;
#define va_start(ap, last) \
    ((ap) = (char *)__builtin_next_arg(last) - \
     (__builtin_args_info(2) < 8 ? (8 - __builtin_args_info(2)) * 8 : 0))
#define va_end(ap) ((void)0)
struct _reent { int _errno; void *_stdin; void *_stdout; };
extern int sub__0000000000000000Inf(struct _reent *, void *, const char *, va_list);

int func_00122B18(struct _reent *ptr, const char *fmt, ...)
{
  int ret;
  va_list ap;

  va_start (ap, fmt);
  ret = sub__0000000000000000Inf (ptr, ptr->_stdout, fmt, ap);
  va_end (ap);
  return ret;
}
