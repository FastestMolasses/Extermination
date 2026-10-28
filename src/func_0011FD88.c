// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 88.66% NEARMISS).
// Fixes read from the original instructions:
// - The body was wrong: the init call takes the stream's reentrancy record
//   (fp+0x54), not the flags; and fp+0x8 receives 0 or the buffer size
//   (fp+0x14) depending on the line/unbuffered flag bits, while the write
//   pointer stays the buffer base (the old C overwrote it with the size).
// - The "register-pair swap" was a symptom of that wrong body, not a wall.
// - The init check is a do { ... } while (0) block (newlib's CHECK_INIT
//   macro shape); gcc aligns its inner label, giving the original's lone nop.
//
// newlib-style fflush: with a null stream, walk every open stream through
// func_001209B0 calling this function; otherwise make sure the stdio state is
// initialised, and if the stream is in write mode with a buffer, write out the
// pending bytes through the stream's write callback, setting the error flag
// (0x40) and returning -1 on a failed write.

struct sbuf {
    unsigned char *_base;   /* 0x10 */
    int _size;              /* 0x14 */
};

struct reent {
    char pad[0x38];
    int sdidinit;           /* 0x38 */
};

struct file {
    unsigned char *_p;      /* 0x00 */
    int _r;                 /* 0x04 */
    int _w;                 /* 0x08 */
    short _flags;           /* 0x0C */
    short _file;            /* 0x0E */
    struct sbuf _bf;        /* 0x10 */
    int _lbfsize;           /* 0x18 */
    void *_cookie;          /* 0x1C */
    int (*_read)(void *, char *, int);  /* 0x20 */
    int (*_write)(void *, const char *, int); /* 0x24 */
    char pad28[0x54 - 0x28];
    struct reent *_data;    /* 0x54 */
};

extern struct reent *D_0024295C;
extern int func_001209B0(struct reent *, int (*)(struct file *));
extern void func_00120058(struct reent *);

int func_0011FD88(struct file *fp)
{
    unsigned char *p;
    int n, t;

    if (fp == 0)
        return func_001209B0(D_0024295C, func_0011FD88);

    do {
        if (fp->_data == 0)
            fp->_data = D_0024295C;
        if (!fp->_data->sdidinit)
            func_00120058(fp->_data);
    } while (0);

    t = fp->_flags;
    if ((t & 0x8) == 0 || (p = fp->_bf._base) == 0)
        return 0;
    n = fp->_p - p;
    fp->_p = p;
    fp->_w = t & (0x1 | 0x2) ? 0 : fp->_bf._size;
    while (n > 0) {
        t = (*fp->_write)(fp->_cookie, (char *)p, n);
        if (t <= 0) {
            fp->_flags |= 0x40;
            return -1;
        }
        p += t;
        n -= t;
    }
    return 0;
}
