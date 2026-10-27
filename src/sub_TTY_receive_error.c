// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.11% NEARMISS).
// Fixes read from the original instructions:
// - case 3's error report also passes the negative result: at that call the
//   second argument register already holds it, which is why the result lives
//   in that register (the old "coloring wall" was a missing argument).
// - case 1/2 reads the write offset into a block-local before forming the
//   SIF buffer address (offset loaded first, buffer base added to it).

// TTY/SIF service dispatcher (ee-gcc SDK region): switch on the request code
// and either push received bytes through the SIF DMA, drain a packet into the
// ring buffer, flush a pending send, or report a leftover send count.

struct ring {
    int size;    /* 0x00 */
    int count;   /* 0x04 */
    int read;    /* 0x08 */
    char *write; /* 0x0C */
};

struct tty {
    int f00;             /* 0x00 */
    volatile int f04;    /* 0x04 */
    volatile int f08;    /* 0x08 */
    volatile int f0C;    /* 0x0C */
    int f10;             /* 0x10 */
    unsigned char *f14;  /* 0x14 */
    struct ring *f18;    /* 0x18 */
};

extern const char D_0026B7D8[];
extern const char D_0026B800[];
extern const char D_0026B818[];
extern const char D_0026B830[];

extern int func_0010C8B0(int a0, int a1, int a2);
extern int func_0010C8E8(int a0, int a1, int a2);
extern void func_0010C9F0(struct ring *r);
extern void func_0010D890(const char *fmt, ...);

void sub_TTY_receive_error(int code, int len, struct tty *tty)
{
    unsigned short *pkt;
    int n;

    switch (code) {
    case 1:
    case 2:
        if (len != 0) {
            if ((unsigned int)(tty->f08 + len) > 0x140) {
                func_0010D890(D_0026B7D8);
            }
            {
            int off = tty->f08;
            len = func_0010C8B0(tty->f00, (int)(tty->f14 + off),
                                len & 0xFFFF);
            }
            if (len < 0) {
                func_0010D890(D_0026B800);
            }
            tty->f08 += len;
        } else {
            pkt = (unsigned short *)tty->f14;
            for (len = 12; len < *pkt; len++) {
                *tty->f18->write = tty->f14[len];
                func_0010C9F0(tty->f18);
            }
            tty->f08 = 0;
        }
        break;
    case 3:
        n = func_0010C8E8(tty->f00, tty->f10, tty->f04 & 0xFFFF);
        if (n < 0) {
            func_0010D890(D_0026B818, n);
            tty->f0C = 0;
        } else {
            tty->f10 += n;
            tty->f04 -= n;
        }
        break;
    case 4:
        if (tty->f04 != 0) {
            func_0010D890(D_0026B830, tty->f04);
        }
        tty->f0C = 0;
        break;
    }
}
