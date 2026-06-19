// CFLAGS: -O4,p -sdatathreshold 0
extern char D_700038A0[0x10];
extern char D_008105E0[0x10];
extern char D_008105D0[0x10];
extern void func_001028D0(char *, char *, char *);
extern float func_00102738(char *, char *);
extern float func_0011E748(float);

int func_00193660(void) {
    float r;
    func_001028D0(D_700038A0, D_008105E0, D_008105D0);
    r = func_0011E748(func_00102738(D_700038A0, D_700038A0));
    *(volatile float *)0x70003A20 = r;
    if (r < 5.5f)
        return 1;
    return 0;
}
