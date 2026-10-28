// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Movie library (SDK libmpeg): returns 1 when the second word of the
// context's internal state block (+0x40) is zero, else 0.

typedef struct {
    int pad0;
    int pending;
} MpegState;

typedef struct {
    int pad[16];
    MpegState *state;
} MpegCtx;

int func_00109A40(MpegCtx *m) {
    return m->state->pending == 0;
}
