// COMPILER: eegcc
// CFLAGS: -O2

extern int sub_image_buffer_needs_to_be_aligned(int *);

int func_00109918(int *arg0, unsigned int arg1, unsigned int arg2) {
    char *p = *(char **)((char *)arg0 + 0x40);
    *(unsigned int *)(p + 0xB0) = 1;
    *(unsigned int *)(p + 0xD8) = (arg1 & 0xFFFFFFF) | 0x20000000;
    *(unsigned int *)(p + 0xE4) = arg2;
    *(unsigned int *)(p + 0xE0) = 0;
    *(unsigned int *)(p + 0xDC) = 0;
    return sub_image_buffer_needs_to_be_aligned(arg0);
}
