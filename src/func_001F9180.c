// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Ground-decal forwarder, twin of func_001F9100 / func_001F9140: passes
// (owner, contact, normal) straight through to func_001F8D30 with the facing
// quad D_0025DB10 copied to the stack (passed by value, so the callee gets a
// pointer to the copy), the colour D_0025DAE0, half width = half height = the
// float argument and fade range 0.0 (no height fade).

typedef struct { float x, y, z, w; } Vec4;
extern void func_001F8D30(void *a0, void *a1, void *a2, Vec4 v, void *p, float f0, float f1, float f2);
extern Vec4 D_0025DB10;
extern char D_0025DAE0[];

void func_001F9180(void *a0, void *a1, void *a2, float fa0) {
    Vec4 v = D_0025DB10;
    func_001F8D30(a0, a1, a2, v, D_0025DAE0, fa0, fa0, 0.0f);
}
