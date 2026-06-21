// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Classifies func_0011E860's return value into 0/1/2/3 by ascending thresholds.
extern int func_0011E860(int);

int func_001B5DC0(int a0) {
    int v = func_0011E860((a0 & 0xFF) - 0x80);
    if (v < 0x31) return 0;
    if (v < 0x59) return 1;
    if (v < 0x7B) return 2;
    return 3;
}
