// COMPILER: eegcc
// CFLAGS: -O2
// if (a0 == 0xA) { func_0010CEE8(0xD); func_0010CEE8(0xA); } else func_0010CEE8(a0).
extern void func_0010CEE8(int a0);

void func_0010CFD0(int a0) {
    if (a0 == 0xA) {
        func_0010CEE8(0xD);
        func_0010CEE8(0xA);
    } else {
        func_0010CEE8(a0);
    }
}
