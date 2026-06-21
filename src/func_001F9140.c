// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Same struct-by-value forwarder as func_001F9100: (a0,a1,a2) forwarded, 16-byte
// D_0025DB00 passed by value as 4th arg (hidden ptr in a3-reg), the incoming 4th
// param `a3` forwarded to p (-> paddub t0,a3,zero / t0-reg), floats f12,f13(=f12),
// f14(=0.0f). Local `Vec4 v = D_0025DB00;` forces the stack qword copy.
typedef struct { float x, y, z, w; } Vec4;
extern void func_001F8D30(void *a0, void *a1, void *a2, Vec4 v, void *p, float f0, float f1, float f2);
extern Vec4 D_0025DB00;

void func_001F9140(void *a0, void *a1, void *a2, void *a3, float fa0) {
    Vec4 v = D_0025DB00;
    func_001F8D30(a0, a1, a2, v, a3, fa0, fa0, 0.0f);
}
