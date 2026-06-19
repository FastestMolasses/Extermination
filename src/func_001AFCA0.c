// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001AF5C0(void);
extern void func_001AF690(void);
extern void func_001AF710(void);
extern void func_001AF8E0(void);
extern void func_001D0660(void);

void func_001AFCA0(void) {
    func_001AF5C0();
    func_001AF690();
    func_001AF710();
    func_001AF8E0();
    func_001D0660();
    *(volatile int *)0x700031F4 = 0;
}
