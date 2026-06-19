// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): apply a vec3 across three setters (tail-call last).
extern void func_00102A60(void *obj, float v);
extern void func_00102BB0(void *obj, void *obj2, float v);
extern void func_00102B08(void *obj, void *obj2, float v);

void func_00102C58(void *obj, int unused, float *v) {
    func_00102A60(obj, v[2]);
    func_00102BB0(obj, obj, v[1]);
    func_00102B08(obj, obj, v[0]);
}
