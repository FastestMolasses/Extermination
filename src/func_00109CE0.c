// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Movie library (SDK libmpeg): for a three-word record returns
// first + second - third.

typedef struct {
    int a;
    int b;
    int c;
} Trio;

int func_00109CE0(Trio *t) {
    return t->a + t->b - t->c;
}
