// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001F03D0(int a0);
extern void func_001F3FA0(void);

void func_001F0310(void) {
    func_001F3FA0();
    func_001F03D0(0);
    func_001F03D0(1);
    func_001F03D0(3);
    func_001F03D0(4);
    func_001F03D0(5);
    func_001F03D0(6);
}
