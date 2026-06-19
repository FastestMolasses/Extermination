// COMPILER: eegcc
// CFLAGS: -O2
extern void func_00120038(void);
extern void init_vtable_a0_at_0011FE90(void *a, int b, int c, void *d);

void func_00120058(int a0) {
    int s1 = a0 + 0x1E4;
    *(int *)(a0 + 0x3C) = (int)func_00120038;
    *(int *)(a0 + 0x38) = 1;
    init_vtable_a0_at_0011FE90((void *)s1, 4, 0, (void *)a0);
    init_vtable_a0_at_0011FE90((void *)(a0 + 0x23C), 9, 1, (void *)a0);
    init_vtable_a0_at_0011FE90((void *)(a0 + 0x294), 0xA, 2, (void *)a0);
    *(int *)(a0 + 0x1E0) = s1;
    *(int *)(a0 + 0x1DC) = 3;
    *(int *)(a0 + 0x1D8) = 0;
}
