// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_00102948(void *, char *);
extern int func_0019BA80(char *, void *, char *, int);
extern char D_700038A0[];

unsigned char func_00176F90(char *arg0) {
    float k = -0.2f;
    func_00102948(&D_700038A0, arg0 + 0xB0);
    *(float *)0x700038A4 = *(float *)0x700038A4 + k;
    if (func_0019BA80(arg0, &D_700038A0, arg0 + 0x280, 7) != 0) {
        *(unsigned char *)(arg0 + 0x23B) = *(unsigned char *)(*(char **)0x700031D0 + 0x1A);
    } else if (*(unsigned char *)(arg0 + 0x23B) != 0x35) {
        *(unsigned char *)(arg0 + 0x23B) = 0;
    }
    return *(unsigned char *)(arg0 + 0x23B);
}
