// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Leaf zeroing loop: clears two int arrays 0x4000 bytes apart, 0x1000 ints each
// (a paired per-element clear of the structure at p and at p+0x4000). mwcc 2.3.3
// reproduces the `paddub a1,zero,zero` zero-init idiom that 991202 does not (60.5%).
void func_001CB5C0(int *p) {
    int i;
    for (i = 0; i < 0x1000; i++) {
        p[0] = 0;
        *(int *)((char *)p + 0x4000) = 0;
        p++;
    }
}
