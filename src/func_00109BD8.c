// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Movie library (SDK libmpeg): returns the word at +0xCC of the context's
// internal state block (+0x40). Sibling of func_00109BE8 (+0xD0) and the
// setter func_00109BC0 (+0xD8).

typedef struct {
    int pad[51];
    int valCC;
    int valD0;
} MpegState;

typedef struct {
    int pad[16];
    MpegState *state;
} MpegCtx;

int func_00109BD8(MpegCtx *m) {
    return m->state->valCC;
}
