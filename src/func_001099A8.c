// COMPILER: eegcc
// CFLAGS: -O2
extern int sub_image_buffer_needs_to_be_aligned(int);

int func_001099A8(int a0, unsigned int a1, int a2, int a3) {
    int *v1 = *(int **)(a0 + 0x40);
    *(int *)((char *)v1 + 0xE0) = a3 << 4;
    *(int *)((char *)v1 + 0xD8) = (a1 & 0x0FFFFFFF) | 0x20000000;
    *(int *)((char *)v1 + 0xE4) = a2 * a3;
    *(int *)((char *)v1 + 0xDC) = a2 << 4;
    *(int *)((char *)v1 + 0xB0) = 0;
    return sub_image_buffer_needs_to_be_aligned(a0);
}
