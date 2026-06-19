// COMPILER: eegcc
// CFLAGS: -O2
// Allocate via func_0010E338(a1), copy fields from s, set DMA tags, then
// tail-call func_0010DE78.
extern int *func_0010E338(int a0);
extern void func_0010DE78(int a0, int a1, int a2, int a3, int a4, int a5);

void func_0010E460(int *s, int a1) {
    int *p = func_0010E338(a1);
    int f14 = s[0x14 / 4];
    int f1C = s[0x1C / 4];
    p[0x14 / 4] = f14;
    p[0x1C / 4] = f1C;
    p[0x20 / 4] = 0x8000000C;
    func_0010DE78(0x80000008, (int)p, 0x40, s[0x20 / 4], s[0x24 / 4], s[0x28 / 4]);
}
