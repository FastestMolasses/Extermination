// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 74.68% NEARMISS).
// Fixes read from the original instructions:
//  - The callee prototypes were wrong: SignalSema takes the semaphore id by value,
//    and the SIF RPC call takes pointer arguments (as in the byte-matched siblings
//    func_00113998 / func_00113B50).
//  - The 8-byte result copy is a memcpy from the uncached receive buffer (gcc
//    expands it inline as an unaligned 64-bit copy); the uncached result word is
//    read into a local before SignalSema, as the siblings do.
//  - The previous "frame stride / coloring wall" was caused by these input defects.
//
// SIF RPC client wrapper (same family as func_00113998): checks the client with
// func_00112E28(0xF), optionally prints a debug message, calls RPC function 1 with
// no send data and a 0x10-byte receive buffer (D_0027AB40). On RPC failure it
// releases the semaphore and returns 0; otherwise it copies 8 bytes from receive
// buffer word 1 (uncached) to p, prints a second debug message, releases the
// semaphore and returns receive buffer word 0.

extern int func_00112E28(int a0);
extern void func_00122B58(char *s);
extern int func_0010E8A8(void *a, int b, int c, void *d, int e, void *f, int g, int h, int i);
extern int SignalSema(int a0);
extern int D_00241CF8;
extern char D_0026BD88[];
extern char D_0026BDA8[];
extern int D_0027AB40[];
extern int D_0027AF60[];
extern int D_00241D0C;
extern void *memcpy(void *, const void *, unsigned int);

int func_00113A58(void *p) {
    int v;
    if (func_00112E28(0xF) == 0) {
        return 0;
    }
    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BD88);
    }
    if (func_0010E8A8(D_0027AF60, 1, 0, 0, 0, D_0027AB40, 0x10, 0, 0) < 0) {
        SignalSema(D_00241D0C);
        return 0;
    }
    memcpy(p, (void *)((int)&D_0027AB40[1] | 0x20000000), 8);
    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BDA8);
    }
    v = *(int *)((int)&D_0027AB40[0] | 0x20000000);
    SignalSema(D_00241D0C);
    return v;
}
