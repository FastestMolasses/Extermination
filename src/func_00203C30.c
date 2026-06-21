// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Early-out then offset compute. Clean-store/branch-delay case:
// 991202 fills the beqz delay slot with `paddub v0,zero,zero` (94.0%);
// mwcc 2.3.3 leaves the nop and byte-matches.
extern int func_00203B80(int);

int func_00203C30(int p) {
    int n;
    if (func_00203B80(p) != 0) {
        return 0;
    }
    n = *(int *)(p + 8);
    return *(int *)(p + 0) + ((n * 75) << 14);
}
