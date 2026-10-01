// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern char D_700038C0[];
extern char D_700038D0[];
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_0019B4C0(char *, void *, void *, int);

void func_001C2540(char *e, void *u, void *v, void *m) {
    func_001026A0(D_700038C0, m, u);
    func_001028B8(D_700038C0, D_700038C0, e + 0xB0);
    func_001026A0(D_700038D0, m, v);
    func_0019B4C0(e, D_700038C0, D_700038D0, 0x80000006);
}
