// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 96.21% NEARMISS).
// Fixes read from the original instructions:
//  - The function is newlib's __sfvwrite written in newlib's own shape: the
//    residual is first read into len (`if ((len = uio->resid) == 0)`), and len
//    is reset to 0 after the iov pointer is fetched; p, len and nldist carry no
//    initialisers. The old C initialised p, len and nldist at declaration,
//    which gave ee-gcc three extra live values and moved every register of the
//    three write loops.
//  - The unbuffered chunk is MIN(len, BUFSIZ) spelled as `len < 1024 ? len : 1024`,
//    and the comparisons against w are the plain size_t/int ones of the
//    original source (no casts).

// libc (newlib stdio): __sfvwrite, writes a gather list (uio: iov array +0,
// count +4, residual bytes +8) to a FILE (fields: p +0, r +4, w +8, flags
// +0xC, buffer base +0x10, buffer size +0x14, cookie +0x1C, write hook
// +0x24). Nothing to write returns 0. A stream that cannot be written
// (func_00124F58, the write set-up, fails when flag 0x8 or the buffer is
// missing) returns -1.
//  Unbuffered (flag 0x2): each iov goes straight to the write hook in
//    pieces of at most 1024 bytes.
//  Fully buffered: a string stream (flag 0x200) copies what fits; otherwise
//    a partly full buffer is topped up and flushed (func_0011FD88), a
//    request smaller than the buffer is copied in, and anything larger is
//    written directly.
//  Line buffered (flag 0x1): each iov is cut at its next newline
//    (func_001216F8 memchr) and flushed after the line ends.
// Any write error sets the error flag 0x40 and returns -1; 0 when all of
// the residual has been written. func_00121920 is memmove.
typedef struct Iov {
    char *base;
    unsigned int len;
} Iov;

typedef struct Uio {
    Iov *iov;
    int count;
    int resid;
} Uio;

typedef struct File {
    char *p;
    int r;
    int w;
    unsigned short flags;
    short file;
    char *base;
    int size;
    int lbfsize;
    void *cookie;
    int (*read)(void *, char *, int);
    int (*write)(void *, const char *, int);
} File;

extern int func_00124F58(File *fp);
extern int func_0011FD88(File *fp);
extern void *func_001216F8(const void *s, int c, unsigned int n);
extern void *func_00121920(void *dst, const void *src, unsigned int n);

int func_001205D8(File *fp, Uio *uio) {
    unsigned int len;
    char *p;
    Iov *iov;
    int w;
    int s;
    char *nl;
    int nlknown;
    int nldist;

    if ((len = uio->resid) == 0) {
        return 0;
    }
    if ((!(fp->flags & 8) || fp->base == 0) && func_00124F58(fp)) {
        return -1;
    }
    iov = uio->iov;
    len = 0;
    if (fp->flags & 2) {
        do {
            while (len == 0) {
                p = iov->base;
                len = iov->len;
                iov++;
            }
            w = fp->write(fp->cookie, p, len < 1024 ? len : 1024);
            if (w <= 0) {
                goto err;
            }
            p += w;
            len -= w;
        } while ((uio->resid -= w) != 0);
    } else if ((fp->flags & 1) == 0) {
        do {
            while (len == 0) {
                p = iov->base;
                len = iov->len;
                iov++;
            }
            w = fp->w;
            if (fp->flags & 0x200) {
                if (len < w) {
                    w = len;
                }
                func_00121920(fp->p, p, w);
                fp->w -= w;
                fp->p += w;
                w = len;
            } else if (fp->p > fp->base && len > w) {
                func_00121920(fp->p, p, w);
                fp->p += w;
                if (func_0011FD88(fp)) {
                    goto err;
                }
            } else if (len >= (w = fp->size)) {
                w = fp->write(fp->cookie, p, w);
                if (w <= 0) {
                    goto err;
                }
            } else {
                w = len;
                func_00121920(fp->p, p, w);
                fp->w -= w;
                fp->p += w;
            }
            p += w;
            len -= w;
        } while ((uio->resid -= w) != 0);
    } else {
        nlknown = 0;
        do {
            while (len == 0) {
                nlknown = 0;
                p = iov->base;
                len = iov->len;
                iov++;
            }
            if (!nlknown) {
                nl = func_001216F8(p, '\n', len);
                nldist = nl ? nl + 1 - p : len + 1;
                nlknown = 1;
            }
            s = len < nldist ? len : nldist;
            w = fp->w + fp->size;
            if (fp->p > fp->base && s > w) {
                func_00121920(fp->p, p, w);
                fp->p += w;
                if (func_0011FD88(fp)) {
                    goto err;
                }
            } else if (s >= (w = fp->size)) {
                w = fp->write(fp->cookie, p, w);
                if (w <= 0) {
                    goto err;
                }
            } else {
                w = s;
                func_00121920(fp->p, p, w);
                fp->w -= w;
                fp->p += w;
            }
            if ((nldist -= w) == 0) {
                if (func_0011FD88(fp)) {
                    goto err;
                }
                nlknown = 0;
            }
            p += w;
            len -= w;
        } while ((uio->resid -= w) != 0);
    }
    return 0;

err:
    fp->flags |= 0x40;
    return -1;
}
