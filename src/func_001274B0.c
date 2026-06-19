// COMPILER: eegcc
// CFLAGS: -O2
extern void func_00126BE8(void *src, void *dst);
extern void func_00127398(void *a0, void *a1);
struct buf { long a, b, c, d; };
void func_001274B0(long a0, long a1) {
    struct buf out0;
    struct buf out1;
    long in0;
    long in1;
    in0 = a0;
    in1 = a1;
    func_00126BE8(&in0, &out0);
    func_00126BE8(&in1, &out1);
    func_00127398(&out0, &out1);
}
