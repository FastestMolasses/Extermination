// NEARMISS func_0010CE28  (vram 0x0010CE28, 0xBC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.70% via ee-gcc 2.9-991111-01 (-O2) (was 81.64%). The header packet's
// reserved halfword (+2) is cleared before its +8 word in the original (it lands in
// the ring-init call's delay slot); that order fix raised the score.
// Remaining differences: pure ee-gcc register colouring and store scheduling in the
// context setup: the original keeps the 0x20000000 uncached mask in v1 and the read
// buffer address in a0 (ours swaps them), and stores the three zero fields before the
// read-buffer pointer. Insensitive to statement order, locals, struct vs pointer
// spelling, mask local, unsigned casts, EEGCC_AS 2.96 and other CFLAGS.
//
// SDK DECI2 TTY init: flush the data cache, open DECI2 protocol 0x210 with the
// context record and the receive handler, fail (0) if the socket is negative;
// otherwise clear the length/busy fields, point the write and read buffers at the
// uncached views of their static buffers, prefill the write packet header
// (protocol 0x210, source 'E', destination 'H', tty id 0), create the 0x100-byte
// receive ring and return 1.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

struct deci2_hdr {
    /* 0x0 */ unsigned short len;
    /* 0x2 */ unsigned short rsvd;
    /* 0x4 */ unsigned short proto;
    /* 0x6 */ unsigned char src;
    /* 0x7 */ unsigned char dst;
    /* 0x8 */ int tty;
};

struct tty_ctx {
    /* 0x00 */ int socket;
    /* 0x04 */ int wlen;
    /* 0x08 */ int rlen;
    /* 0x0C */ int busy;
    /* 0x10 */ struct deci2_hdr *wbuf;
    /* 0x14 */ char *rbuf;
    /* 0x18 */ void *ring;
};

extern int FlushCache(int chan);
extern int func_0010C7E8(unsigned short a0, int a1, int a2);
extern int *func_0010C9C8(int a0);
extern void sub_TTY_receive_error(void);
extern struct tty_ctx D_00276E10;
extern char D_00276E40[];
extern char D_00276F80[];

int func_0010CE28(void) {
    struct deci2_hdr *h;

    FlushCache(0);
    D_00276E10.socket = func_0010C7E8(0x210, (int)&D_00276E10, (int)sub_TTY_receive_error);
    if (*(volatile int *)&D_00276E10.socket < 0)
        return 0;
    D_00276E10.busy = 0;
    D_00276E10.wlen = 0;
    D_00276E10.rlen = 0;
    D_00276E10.rbuf = (char *)((unsigned int)D_00276F80 | 0x20000000);
    D_00276E10.wbuf = h = (struct deci2_hdr *)((unsigned int)D_00276E40 | 0x20000000);
    h->proto = 0x210;
    h->src = 0x45;
    h->dst = 0x48;
    h->rsvd = 0;
    h->tty = 0;
    D_00276E10.ring = func_0010C9C8(0x100);
    return 1;
}
