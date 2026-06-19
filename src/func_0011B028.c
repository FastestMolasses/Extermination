// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.96): read a 16-bit value via func_0011AE40 into a local.
extern void func_0011AE40(unsigned short *out, int id);

unsigned short func_0011B028(void) {
    unsigned short v;
    func_0011AE40(&v, 0x43A);
    return v;
}
