// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// stream_archive: 0x20 x 0x20 grid initializer. Outer row stride 0x200, inner
// column stride 0x10; stores the float arg to +0x64 of each cell and (per inner
// iteration) re-stores it to +0x4 of the base pointer p.
//
// Built with mwcc 2.3.3, not 991202. Sequence/scheduling matched at the body
// level; the residual was pure register-allocation order. Declaration order
// (j, i, col, row) steers mwcc's saved/temp assignment to CW's (i=a2, row=t0,
// j=a1, col=a3). do/while loops give the fall-through inits. objdiff 100.0.
void func_001E7C60(char *p, float v) {
    int j;
    int i;
    char *col;
    char *row;

    i = 0;
    row = p;
    do {
        j = 0;
        col = row;
        do {
            j++;
            *(float *)(col + 0x64) = v;
            col += 0x10;
            *(float *)(p + 4) = v;
        } while (j < 0x20);
        i++;
        row += 0x200;
    } while (i < 0x20);
}
