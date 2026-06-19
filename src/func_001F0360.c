// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001F6210(void);
extern void func_001F5C20(void);
extern void func_001F6BB0(void);
extern void func_001F6EB0(void);
extern void func_001F40C0(void);
extern void func_001F0720(int a0);

void func_001F0360(void) {
    func_001F6210();
    func_001F5C20();
    func_001F6BB0();
    func_001F6EB0();
    func_001F40C0();
    func_001F0720(0);
    func_001F0720(1);
    func_001F0720(3);
    func_001F0720(4);
    func_001F0720(5);
    func_001F0720(6);
}
