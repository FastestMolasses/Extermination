// CFLAGS: -O4,p -sdatathreshold 4
extern char *D_00275670;

typedef struct { int a, b, c, d; } Node16;

void func_001D71A0(int a0, int a1) {
    char *a3 = D_00275670 + (a0 << 2);
    int v1 = a1 | 0x14000000;
    char *ptr = *(char **)(a3 + 0x10);
    ptr[3] = 0x10;
    ptr = *(char **)(a3 + 0x10);
    *(int *)(ptr + 4) = 0;
    ptr = *(char **)(a3 + 0x10);
    *(short *)ptr = 1;
    ptr = *(char **)(a3 + 0x10);
    *(char **)(a3 + 0x10) = ptr + 0x20;
    ((Node16 *)(ptr + 0x10))->a = 0;
    ((Node16 *)(ptr + 0x10))->b = 0;
    ((Node16 *)(ptr + 0x10))->c = 0;
    ((Node16 *)(ptr + 0x10))->d = v1;
}
