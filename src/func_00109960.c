// COMPILER: eegcc
// CFLAGS: -O2
extern int sub_image_buffer_needs_to_be_aligned(int *a0);

int func_00109960(int *a0, unsigned int a1, int a2) {
    int *p = (int *)a0[16];
    p[0x39] = a2;
    p[0x36] = (a1 & 0xFFFFFFF) | 0x20000000;
    p[0x2C] = 0;
    p[0x38] = 0;
    p[0x37] = 0;
    return sub_image_buffer_needs_to_be_aligned(a0);
}
