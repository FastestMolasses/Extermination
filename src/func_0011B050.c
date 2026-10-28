// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// SDK VU1 context save: unless func_0011ADD0 reports a VU running (then returns
// 0), reads every VU1 register through VU0 memory with func_0011AE40 and stores it
// into ctx: vf0..vf31 (addresses 0x400..0x41F), status, MAC, clip, R, I, Q and P
// (0x430, 0x431, 0x432, 0x434, 0x435, 0x436, 0x437; low word of each) and
// vi0..vi15 (0x420..0x42F; low halfword). Returns 1.

typedef unsigned int u128 __attribute__((mode(TI)));
typedef struct {
    unsigned long d[2];
} vu_qword;
typedef union {
    vu_qword s;
    u128 q;
} vu_qword_u;

struct vu1_context {
    u128 vf[32];
    unsigned int status;
    unsigned int mac;
    unsigned int clip;
    unsigned int r;
    unsigned int i;
    unsigned int q;
    unsigned int p;
    unsigned short vi[16];
};

extern int func_0011ADD0(void);
extern vu_qword func_0011AE40(unsigned short addr);

int func_0011B050(struct vu1_context *ctx) {
    vu_qword vf, mac, clip, r, i, q, p, vi;
    unsigned int n;

    if (func_0011ADD0() != 0) {
        return 0;
    }
    for (n = 0; n < 32; n++) {
        vf = func_0011AE40(0x400 + n);
        ctx->vf[n] = *(u128 *)&vf;
    }
    vf = func_0011AE40(0x430);
    ctx->status = vf.d[0];
    mac = func_0011AE40(0x431);
    ctx->mac = mac.d[0];
    clip = func_0011AE40(0x432);
    ctx->clip = clip.d[0];
    r = func_0011AE40(0x434);
    ctx->r = r.d[0];
    i = func_0011AE40(0x435);
    ctx->i = i.d[0];
    q = func_0011AE40(0x436);
    ctx->q = q.d[0];
    p = func_0011AE40(0x437);
    ctx->p = p.d[0];
    for (n = 0; n < 16; n++) {
        vi = func_0011AE40(0x420 + n);
        ctx->vi[n] = vi.d[0];
    }
    return 1;
}
