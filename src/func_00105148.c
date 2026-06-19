// COMPILER: eegcc
// CFLAGS: -O2
void func_00105148(unsigned int a0) {
    unsigned int *p = (unsigned int *)0x10002010;
    *p = (*p & 0xFF7FFFFF) | (a0 << 23);
}
