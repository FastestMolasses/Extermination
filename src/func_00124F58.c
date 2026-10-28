// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 84.03% NEARMISS).
// Fixes read from the original instructions:
// - This is newlib's __swsetup; written in its own source shape with the
//   FILE-shaped record (as in the matched fflush, func_0011FD88): the stdio
//   init check as a CHECK_INIT-style do { } while (0) block, flag tests read
//   straight from fp->_flags, and the ungetc-buffer free (HASUB/FREEUB) as
//   nested ifs. The old "ee-gcc codegen wall" verdict was wrong.
//
// newlib-style __swsetup: make sure stdio is initialised for the stream's
// reentrancy record (fp+0x54); if the stream is not in write mode, fail with -1
// unless it is read/write, dropping any read state (freeing a separate ungetc
// buffer through func_001200E8, clearing the read/EOF flags 0x24, resetting the
// read count and pointer) before setting the write flag 0x8. Allocate a buffer
// through func_00120B98 when there is none, then set the write count: 0 with a
// negative line-buffer size for line-buffered streams, else 0 for unbuffered
// streams or the buffer size.
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
    int (*_seek)(void *, int, int);     /* 0x28 */
    int (*_close)(void *);  /* 0x2C */
    struct sbuf _ub;        /* 0x30 */
    unsigned char *_up;     /* 0x38 */
    int _ur;                /* 0x3C */
    unsigned char _ubuf[3]; /* 0x40 */
    unsigned char _nbuf[1]; /* 0x43 */
    struct sbuf _lb;        /* 0x44 */
    int _blksize;           /* 0x4C */
    int _offset;            /* 0x50 */
    struct reent *_data;    /* 0x54 */
};

extern struct reent *D_0024295C;
extern void func_00120058(struct reent *);
extern void func_001200E8(struct reent *, void *);
extern void func_00120B98(struct file *);

int func_00124F58(struct file *fp)
{
    do {
        if (fp->_data == 0)
            fp->_data = D_0024295C;
        if (!fp->_data->sdidinit)
            func_00120058(fp->_data);
    } while (0);

    if ((fp->_flags & 0x8) == 0) {
        if ((fp->_flags & 0x10) == 0)
            return -1;
        if (fp->_flags & 0x4) {
            if (fp->_ub._base != 0) {
                if (fp->_ub._base != fp->_ubuf)
                    func_001200E8(fp->_data, fp->_ub._base);
                fp->_ub._base = 0;
            }
            fp->_flags &= ~(0x4 | 0x20);
            fp->_r = 0;
            fp->_p = fp->_bf._base;
        }
        fp->_flags |= 0x8;
    }

    if (fp->_bf._base == 0)
        func_00120B98(fp);

    if (fp->_flags & 0x1) {
        fp->_w = 0;
        fp->_lbfsize = -fp->_bf._size;
    } else
        fp->_w = fp->_flags & 0x2 ? 0 : fp->_bf._size;

    return 0;
}
