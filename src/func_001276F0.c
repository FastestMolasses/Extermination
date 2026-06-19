// COMPILER: eegcc
// CFLAGS: -O2
extern void func_00126BE8(void *src, void *dst);
extern void func_00126AB8(void *p);
void func_001276F0(long a0) {
    long in;
    unsigned int out[8];
    in = a0;
    func_00126BE8(&in, &out[0]);
    out[1] = (out[1] < 1);
    func_00126AB8(&out[0]);
}
