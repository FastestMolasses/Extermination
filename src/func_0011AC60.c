// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// The VU0 macro / COP2 transfers are GCC-style asm statements, one instruction
// each (user-approved 2026-10-28 for the VU0 class); everything else is C.
//
// SDK VU0 context restore (the pair of func_0011AB80): unless func_0011A9C0
// reports VU0 busy (then returns 0), loads the control registers (status, MAC,
// clip, R, I, Q) and vi0..vi15 from ctx into VU0 and returns 1.
// QUIRK (original behaviour): the vf half STORES the current vf0..vf31 into
// ctx->vf (a save, not a load), so the vf registers are never restored and
// ctx->vf is overwritten with the live values.

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
int func_0011AC60(struct vu0_context *ctx) {
    u128 *vf = ctx->vf;
    unsigned int v;

    if (func_0011A9C0() != 0) {
        return 0;
    }
    asm volatile("sqc2 $vf0, 0x0(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf1, 0x10(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf2, 0x20(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf3, 0x30(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf4, 0x40(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf5, 0x50(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf6, 0x60(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf7, 0x70(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf8, 0x80(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf9, 0x90(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf10, 0xa0(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf11, 0xb0(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf12, 0xc0(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf13, 0xd0(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf14, 0xe0(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf15, 0xf0(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf16, 0x100(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf17, 0x110(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf18, 0x120(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf19, 0x130(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf20, 0x140(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf21, 0x150(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf22, 0x160(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf23, 0x170(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf24, 0x180(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf25, 0x190(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf26, 0x1a0(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf27, 0x1b0(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf28, 0x1c0(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf29, 0x1d0(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf30, 0x1e0(%0)" : : "r"(vf) : "memory");
    asm volatile("sqc2 $vf31, 0x1f0(%0)" : : "r"(vf) : "memory");
    v = ctx->status;
    asm volatile("ctc2.ni %0, $vi16" : : "r"(v));
    v = ctx->mac;
    asm volatile("ctc2.ni %0, $vi17" : : "r"(v));
    v = ctx->clip;
    asm volatile("ctc2.ni %0, $vi18" : : "r"(v));
    v = ctx->r;
    asm volatile("ctc2.ni %0, $vi20" : : "r"(v));
    v = ctx->i;
    asm volatile("ctc2.ni %0, $vi21" : : "r"(v));
    v = ctx->q;
    asm volatile("ctc2.ni %0, $vi22" : : "r"(v));
    v = ctx->vi[0];
    asm volatile("ctc2.ni %0, $vi0" : : "r"(v));
    v = ctx->vi[1];
    asm volatile("ctc2.ni %0, $vi1" : : "r"(v));
    v = ctx->vi[2];
    asm volatile("ctc2.ni %0, $vi2" : : "r"(v));
    v = ctx->vi[3];
    asm volatile("ctc2.ni %0, $vi3" : : "r"(v));
    v = ctx->vi[4];
    asm volatile("ctc2.ni %0, $vi4" : : "r"(v));
    v = ctx->vi[5];
    asm volatile("ctc2.ni %0, $vi5" : : "r"(v));
    v = ctx->vi[6];
    asm volatile("ctc2.ni %0, $vi6" : : "r"(v));
    v = ctx->vi[7];
    asm volatile("ctc2.ni %0, $vi7" : : "r"(v));
    v = ctx->vi[8];
    asm volatile("ctc2.ni %0, $vi8" : : "r"(v));
    v = ctx->vi[9];
    asm volatile("ctc2.ni %0, $vi9" : : "r"(v));
    v = ctx->vi[10];
    asm volatile("ctc2.ni %0, $vi10" : : "r"(v));
    v = ctx->vi[11];
    asm volatile("ctc2.ni %0, $vi11" : : "r"(v));
    v = ctx->vi[12];
    asm volatile("ctc2.ni %0, $vi12" : : "r"(v));
    v = ctx->vi[13];
    asm volatile("ctc2.ni %0, $vi13" : : "r"(v));
    v = ctx->vi[14];
    asm volatile("ctc2.ni %0, $vi14" : : "r"(v));
    v = ctx->vi[15];
    asm volatile("ctc2.ni %0, $vi15" : : "r"(v));
    return 1;
}
