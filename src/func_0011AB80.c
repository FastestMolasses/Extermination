// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// The VU0 macro / COP2 transfers are GCC-style asm statements, one instruction
// each (user-approved 2026-10-28 for the VU0 class); everything else is C.
//
// SDK VU0 context save: unless func_0011A9C0 reports VU0 busy (then returns 0
// and saves nothing), copies the VU0 control registers (status, MAC, clip, R, I, Q
// = COP2 control registers 16, 17, 18, 20, 21, 22) and the integer registers
// vi0..vi15 into ctx and returns 1.
// QUIRK (original behaviour): the 32 vf registers are NOT saved here; the store of
// vf0..vf31 into ctx->vf sits in the paired "restore" func_0011AC60 instead.

typedef unsigned int u128 __attribute__((mode(TI)));

struct vu0_context {
    u128 vf[32];
    unsigned int status;
    unsigned int mac;
    unsigned int clip;
    unsigned int r;
    unsigned int i;
    unsigned int q;
    unsigned short vi[16];
};

extern int func_0011A9C0(void);
int func_0011AB80(struct vu0_context *ctx) {
    unsigned int v;

    if (func_0011A9C0() != 0) {
        return 0;
    }
    asm("cfc2.ni %0, $vi16" : "=r"(v));
    ctx->status = v;
    asm("cfc2.ni %0, $vi17" : "=r"(v));
    ctx->mac = v;
    asm("cfc2.ni %0, $vi18" : "=r"(v));
    ctx->clip = v;
    asm("cfc2.ni %0, $vi20" : "=r"(v));
    ctx->r = v;
    asm("cfc2.ni %0, $vi21" : "=r"(v));
    ctx->i = v;
    asm("cfc2.ni %0, $vi22" : "=r"(v));
    ctx->q = v;
    asm("cfc2.ni %0, $vi0" : "=r"(v));
    ctx->vi[0] = v;
    asm("cfc2.ni %0, $vi1" : "=r"(v));
    ctx->vi[1] = v;
    asm("cfc2.ni %0, $vi2" : "=r"(v));
    ctx->vi[2] = v;
    asm("cfc2.ni %0, $vi3" : "=r"(v));
    ctx->vi[3] = v;
    asm("cfc2.ni %0, $vi4" : "=r"(v));
    ctx->vi[4] = v;
    asm("cfc2.ni %0, $vi5" : "=r"(v));
    ctx->vi[5] = v;
    asm("cfc2.ni %0, $vi6" : "=r"(v));
    ctx->vi[6] = v;
    asm("cfc2.ni %0, $vi7" : "=r"(v));
    ctx->vi[7] = v;
    asm("cfc2.ni %0, $vi8" : "=r"(v));
    ctx->vi[8] = v;
    asm("cfc2.ni %0, $vi9" : "=r"(v));
    ctx->vi[9] = v;
    asm("cfc2.ni %0, $vi10" : "=r"(v));
    ctx->vi[10] = v;
    asm("cfc2.ni %0, $vi11" : "=r"(v));
    ctx->vi[11] = v;
    asm("cfc2.ni %0, $vi12" : "=r"(v));
    ctx->vi[12] = v;
    asm("cfc2.ni %0, $vi13" : "=r"(v));
    ctx->vi[13] = v;
    asm("cfc2.ni %0, $vi14" : "=r"(v));
    ctx->vi[14] = v;
    asm("cfc2.ni %0, $vi15" : "=r"(v));
    ctx->vi[15] = v;
    return 1;
}
