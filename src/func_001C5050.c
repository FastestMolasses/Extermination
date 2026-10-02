// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Copies the vector obj+0x80 to a stack vector, sets its w to the given
// value and passes it to func_001D7FA0(obj+0xB0, &v, 2, 1.0, 0.0).
typedef struct { float v[3]; float w; } Vec4;
extern void func_00102948(void *dst, void *src);
extern void func_001D7FA0(void *a0, void *a1, int a2, float f12, float f13);

void func_001C5050(char *obj, float w) {
    Vec4 t;
    func_00102948(&t, obj + 0x80);
    t.w = w;
    func_001D7FA0(obj + 0xB0, &t, 2, 1.0f, 0.0f);
}
