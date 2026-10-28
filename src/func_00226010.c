// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Two-stage gate on an object (self). Polls func_001FE9A0(1, &self->c (+0x50),
// &self->d (+0x54)); a 0 status returns 0. Otherwise returns 1 when
// func_001FE920(self->a (+0x48), self->b (+0x4C)) returns 0, else 0. Same
// shape as the first half of func_00228070. (Callers func_00226070 and
// func_002267A0 declare it void and ignore the result.)
extern int func_001FE920(int a0, int a1);
extern int func_001FE9A0(int a0, int *a1, int *a2);

struct Rec { char pad[0x48]; int a; int b; int c; int d; };
int func_00226010(struct Rec *self) {
    if (func_001FE9A0(1, &self->c, &self->d) == 0) {
        return 0;
    }
    return (func_001FE920(self->a, self->b) == 0) ? 1 : 0;
}
