// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
void func_00206470(unsigned char *p, unsigned int n) {
    unsigned int avail;
    if (*(int *)(p + 0x0) == 0) {
        avail = 0x28 - *(unsigned int *)(p + 0x2C);
        avail = (avail < n) ? avail : n;
        *(unsigned int *)(p + 0x2C) += avail;
        if (*(unsigned int *)(p + 0x2C) >= 0x28) {
            *(int *)(p + 0x0) = 1;
        }
        n -= avail;
    }
    *(int *)(p + 0x34) = (*(int *)(p + 0x34) + (int)n) % *(int *)(p + 0x3C);
    *(unsigned int *)(p + 0x38) += n;
    *(unsigned int *)(p + 0x40) += n;
}
