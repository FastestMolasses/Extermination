// COMPILER: eegcc
// CFLAGS: -O2
// Movie library (SDK libmpeg, title path): sets the picture size of the
// decoder context: +4 / +8 the width and height in pixels, +0xC / +0x10 the
// same in 16-pixel macroblocks. Returns 1.
int func_0010A4D8(char *m, int w, int h) {
    *(int *)(m + 4) = w;
    *(int *)(m + 0xC) = w >> 4;
    *(int *)(m + 8) = h;
    *(int *)(m + 0x10) = h >> 4;
    return 1;
}
