// CFLAGS: -O4,p -sdatathreshold 0
extern int dmac_channel_base(int);
extern void func_00101F08(int, int);
extern void func_00102468(int, int, int);

void func_00200830(int a0) {
    int base = dmac_channel_base(1);
    func_00102468(base, 0, 0);
    func_00101F08(base, a0);
    func_00102468(base, 0, 0);
}
