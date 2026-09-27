// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: query SIF DMA status into a local for channel 4.
extern int Deci2Call(int a0, void *a1);

int func_0010C888(int a0) {
    int local = a0;
    return Deci2Call(4, &local);
}
