// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Returns the top n bits of the 64-bit word at *p (a logical right shift by
// 64 - n), truncated to int.
int func_00108640(unsigned long long *p, int n) {
    return *p >> (64 - n);
}
