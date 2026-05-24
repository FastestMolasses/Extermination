// CFLAGS: -O4,p -sdatathreshold 0
typedef struct { int x, y, z, w; } Vec4;
extern Vec4 D_0025DB00;
extern void func_001F8D30(int, Vec4 *, float, float);

void func_001F9140(int a0, int a1, int a2, int a3) {
    Vec4 buf;
    buf = D_0025DB00;
    func_001F8D30(a2, &buf, *(float *)&a0, 0.0f);
}
