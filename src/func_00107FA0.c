// COMPILER: eegcc
// CFLAGS: -O2

typedef union { int w; long d; } EEWord;

extern int D_002412F4;
extern int D_002414AC;

extern void func_00107CF0(char *ctx, long *a, long *b, long *c);
extern int func_00107A28(char *ctx);
extern void func_001084B0(char *ctx);
extern void func_00107AC8(char *ctx);
extern void func_00107CB8(void);

void func_00107FA0(char *a, char *b)
{
    char *base;
    char *base2;
    char *base3;
    char *dev;
    char *x;
    char *y;
    int flag;
    int t;
    int t2;

    flag = 0;
    dev = *(char **)(D_002412F4 + 0x40);
    if (D_002414AC == 2) {
        x = a;
        y = b;
        flag = 0x40;
    } else {
        x = b;
        y = a;
    }

    base = (char *)D_002412F4;
    func_00107CF0(x, (long *)(base + 0x10), (long *)(base + 0x18), (long *)(base + 0x20));

    base2 = (char *)D_002412F4;
    t = *(int *)(base2 + 0x10);
    ((EEWord *)(dev + 0x88))->d = 1;
    *(int *)(dev + 0x80) = t;
    func_00107CF0(y, (long *)(base2 + 0x28), (long *)(base2 + 0x30), (long *)(base2 + 0x38));

    base3 = (char *)D_002412F4;
    t2 = *(int *)(base3 + 0x28);
    ((EEWord *)(dev + 0x88))->d = 1;
    *(int *)(dev + 0x80) = t2;
    *(long *)(base3 + 0x20) |= flag;
    *(int *)(dev + 0xCC) = *(int *)(x + 0x5C);
    *(long *)(base3 + 0x38) |= flag;
    *(int *)(dev + 0xD0) = *(int *)(x + 0x60);
    *(int *)(dev + 0xB4) = *(int *)(x + 0x44);
    *(int *)(dev + 0xB8) = *(int *)(y + 0x48);
    *(int *)(dev + 0xC0) = *(int *)(x + 0x50);
    *(int *)(dev + 0xC4) = *(int *)(y + 0x54);

    if (func_00107A28(a) == 0)
        return;
    if (*(int *)(a + 0x28) != 1)
        return;
    if (*(int *)(b + 0x28) != *(int *)(a + 0x28))
        return;

    *(int *)(a + 0x10) = *(int *)(a + 0x10) << 1;
    if (*(int *)(dev + 0xB0) != 0)
        func_001084B0(a);
    else
        func_00107AC8(a);
    *(int *)(a + 0x10) = *(int *)(a + 0x10) >> 1;
    func_00107CB8();
}
