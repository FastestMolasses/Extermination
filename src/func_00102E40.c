// COMPILER: eegcc
// CFLAGS: -O2
extern void func_00102948(void *dst, void *src);
void func_00102E40(char *a0, void *a1, void *a2, void *a3, void *t0) {
    func_00102948(a0, a1);
    func_00102948(a0 + 0x10, a2);
    func_00102948(a0 + 0x20, a3);
    func_00102948(a0 + 0x30, t0);
}
