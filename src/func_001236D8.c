// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 87.83% NEARMISS).
// Fixes read from the original instructions:
// - The function returns the formatter's int result (newlib vfprintf shape:
//   `return _vfprintf_r(...)`); with a void return ee-gcc turned the tail call
//   into a sibling jump and recoloured the saved registers.
// - The lazy-bind/one-time-init block is newlib's CHECK_INIT macro shape, a
//   do { ... } while (0) block; gcc aligns its inner label, giving the
//   original's lone filler slot after the default store.
// - Typed FILE / reentrancy structs instead of byte offsets (same layout as the
//   byte-matched fflush in src/func_0011FD88.c).
//
// newlib-style vfprintf: lazily binds the stream's reentrancy record (+0x54)
// to the global default D_0024295C, runs the one-time stdio init
// (func_00120058) if that record's init flag (+0x38) is clear, then hands
// (record, stream, format, argument list) to the formatter
// sub__0000000000000000Inf and returns its result.

struct reent {
    char pad[0x38];
    int sdidinit;           /* 0x38 */
};

struct file {
    char pad[0x54];
    struct reent *_data;    /* 0x54 */
};

extern struct reent *D_0024295C;
extern void func_00120058(struct reent *);
extern int sub__0000000000000000Inf(struct reent *, struct file *, const char *, char *);

int func_001236D8(struct file *fp, const char *fmt0, char *ap)
{
    do {
        if (fp->_data == 0)
            fp->_data = D_0024295C;
        if (!fp->_data->sdidinit)
            func_00120058(fp->_data);
    } while (0);
    return sub__0000000000000000Inf(fp->_data, fp, fmt0, ap);
}
