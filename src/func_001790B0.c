// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Seven probes: for each 16-byte entry i of the table D_00248970,
// func_001026A0(0x700038A0, self+0xD0, entry) fills the scratchpad vector
// 0x700038A0 and func_0019AD00(self, 0x700038A0, 0x80000007) tests it; a nonzero
// result sets bit i of self+0x314 (cleared first). Returns self+0x314.
extern char D_00248970[];
extern char D_700038A0[];
extern void func_001026A0(void *a, void *b, void *c);
extern int func_0019AD00(char *, char *, int);

unsigned char func_001790B0(unsigned char *self) {
    int i;
    char *dir = D_00248970;
    self[0x314] = 0;
    for (i = 0; i <= 6; i++) {
        func_001026A0(D_700038A0, self + 0xD0, dir);
        if (func_0019AD00((char *)self, D_700038A0, 0x80000007) != 0) {
            self[0x314] |= 1 << i;
        }
        dir += 0x10;
    }
    return self[0x314];
}
