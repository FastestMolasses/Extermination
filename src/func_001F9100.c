// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Forwards (a0,a1,a2) through to func_001F8D30, plus a 16-byte struct passed BY VALUE
// (D_0025DAF0) as the 4th arg -> invisible pointer in a3-reg; D_0025DAE0 ptr in t0;
// floats f12,f13(=f12),f14(=30.0f). The local `Vec4 v = D_0025DAF0;` copy forces the
// stack qword copy (sq) that the direct-global form would optimize/tailcall away.
typedef struct { float x, y, z, w; } Vec4;
extern void func_001F8D30(void *a0, void *a1, void *a2, Vec4 v, void *p, float f0, float f1, float f2);
extern Vec4 D_0025DAF0;
extern char D_0025DAE0[];

void func_001F9100(void *a0, void *a1, void *a2, float fa0) {
    Vec4 v = D_0025DAF0;
    func_001F8D30(a0, a1, a2, v, D_0025DAE0, fa0, fa0, 30.0f);
}
