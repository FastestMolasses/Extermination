// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Movie library (SDK libmpeg): passes the IPU/DMA save area at +0x4C of the
// context's internal state block (+0x40) to func_0010ADA8, which stops the
// IPU-in/out DMA channels and saves their registers and the IPU control word
// there. A void function whose last statement is the call: ee-gcc turns
// exactly that shape into the original's tail jump.

typedef struct {
    int pad[16];
    char *state;
} MpegCtx;

extern void func_0010ADA8(void *save);

void func_0010AD88(MpegCtx *m) {
    func_0010ADA8(m->state + 0x4C);
}
