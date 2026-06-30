// COMPILER: eegcc
// CFLAGS: -O2
int func_00119080(int a, int b, int c, int d) {
    a &= 0xFF;
    return (a + (((b & 0xFF) - a) * (d & 0xFF)) / (c & 0xFF)) & 0xFF;
}
