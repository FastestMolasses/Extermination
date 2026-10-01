// COMPILER: eegcc
// CFLAGS: -O2
// Movie library (SDK libmpeg, title path): returns the first word of the
// object that obj+0x40 points at.
int func_00109A30(char *obj) {
    return **(int **)(obj + 0x40);
}
