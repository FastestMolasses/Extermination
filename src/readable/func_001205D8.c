// NEARMISS func_001205D8  (vram 0x001205D8, 0x3D8 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 96.21% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc register allocation and loop layout of the three write paths (structure and calls follow
// the original).
//
// The function links from the asm body in src/func_001205D8.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

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
    Iov *iov;
    char *p = 0;
    unsigned int len = 0;
    int w;
    int s;
    char *nl;
    int nlknown;
    int nldist = 0;

    if (uio->resid == 0) {
        return 0;
    }
    if ((!(fp->flags & 8) || fp->base == 0) && func_00124F58(fp) != 0) {
        return -1;
    }
    iov = uio->iov;
    if (fp->flags & 2) {
        do {
            while (len == 0) {
                p = iov->base;
                len = iov->len;
                iov++;
            }
            w = fp->write(fp->cookie, p, len < 0x401 ? len : 0x400);
            if (w <= 0) {
                goto err;
            }
            p += w;
            len -= w;
        } while ((uio->resid -= w) != 0);
    } else if (!(fp->flags & 1)) {
        do {
            while (len == 0) {
                p = iov->base;
                len = iov->len;
                iov++;
            }
            w = fp->w;
            if (fp->flags & 0x200) {
                if (len < (unsigned int)w) {
                    w = len;
                }
                func_00121920(fp->p, p, w);
                fp->w -= w;
                fp->p += w;
                w = len;
            } else if (fp->p > fp->base && len > (unsigned int)w) {
                func_00121920(fp->p, p, w);
                fp->p += w;
                if (func_0011FD88(fp) != 0) {
                    goto err;
                }
            } else if (len >= (unsigned int)(w = fp->size)) {
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
            s = len < (unsigned int)nldist ? len : nldist;
            w = fp->w + fp->size;
            if (fp->p > fp->base && s > w) {
                func_00121920(fp->p, p, w);
                fp->p += w;
                if (func_0011FD88(fp) != 0) {
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
                if (func_0011FD88(fp) != 0) {
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
