// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// SDK VU1 context restore: unless func_0011ADD0 reports a VU running (then
// returns 0), writes ctx back into the VU1 registers through VU0 memory with
// func_0011AE00: vf0..vf31, then status/MAC/clip/R/I/Q/P and vi0..vi15 as
// zero-extended quadwords (one buffer, zeroed once, low doubleword replaced per
// register). Returns 1.

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
extern void func_0011AE00(unsigned short addr, vu_qword *src);

int func_0011B1E0(struct vu1_context *ctx) {
    vu_qword_u t;
    unsigned int n;

    if (func_0011ADD0() != 0) {
        return 0;
    }
    for (n = 0; n < 32; n++) {
        t.q = ctx->vf[n];
        func_0011AE00(0x400 + n, &t.s);
    }
    t.q = 0;
    t.s.d[0] = ctx->status;
    func_0011AE00(0x430, &t.s);
    t.s.d[0] = ctx->mac;
    func_0011AE00(0x431, &t.s);
    t.s.d[0] = ctx->clip;
    func_0011AE00(0x432, &t.s);
    t.s.d[0] = ctx->r;
    func_0011AE00(0x434, &t.s);
    t.s.d[0] = ctx->i;
    func_0011AE00(0x435, &t.s);
    t.s.d[0] = ctx->q;
    func_0011AE00(0x436, &t.s);
    t.s.d[0] = ctx->p;
    func_0011AE00(0x437, &t.s);
    for (n = 0; n < 16; n++) {
        t.s.d[0] = ctx->vi[n];
        func_0011AE00(0x420 + n, &t.s);
    }
    return 1;
}
