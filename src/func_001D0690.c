// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Zero-init of a small record at base a0: clears a byte at +0x10 and words at
// +0x00/+0x08/+0x14/+0x1C/+0x18, then loops 6 times zeroing the word at a0+0x20
// while advancing a0 by 4 each pass (i.e. zeroing +0x20,+0x24,...,+0x34). The
// +0x08 store via a held pointer and the `i <= 5` loop bound are needed to land
// the target's exact register allocation (counter in $v1, slti -> $at; mwcc
// emits a `paddub v1,zero,zero` to materialize the zero counter). Found via the
// permuter; verified objdiff 100% byte-identical vs build/expected.
void func_001D0690(char *a0) {
    int *p;
    int i;
    *(char *)(a0 + 0x10) = 0;
    *(int *)(a0 + 0x00) = 0;
    p = (int *)(a0 + 0x08);
    *p = 0;
    *(int *)(a0 + 0x14) = 0;
    *(int *)(a0 + 0x1C) = 0;
    *(int *)(a0 + 0x18) = 0;
    for (i = 0; i <= 5; i++) {
        *(int *)(a0 + 0x20) = 0;
        a0 += 4;
    }
}
