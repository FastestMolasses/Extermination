// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Clamp-init leaf: store a1 byte at base+0x80; if a1==0, zero 6 ints
// starting at base+0x90 (=base+0x70 +0x20). Counter lands in v1 only when
// the a1!=0 test is hoisted into its own temp (frees a1 from the loop).
void func_001D06E0(int a0, int a1) {
    int base;
    int i;
    int skip;
    int *p;

    base = *(int *)(a0 + 0x90);
    p = (int *)(base + 0x70);
    *(unsigned char *)(base + 0x80) = a1;
    skip = a1 != 0;
    if (skip) {
        return;
    }
    for (i = 0; i <= 6 - 1; i++) {
        p[8] = 0;
        p++;
    }
}
