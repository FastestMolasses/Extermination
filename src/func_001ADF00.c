// CFLAGS: -O4,p -sdatathreshold 4
extern int D_00810D38[2];
extern unsigned char D_00275BDC;
extern void func_001D2880(void);
extern void func_001D1EF0(void);
extern void func_001AEBA0(int);
extern void func_001AB790(void *);
extern void func_001AC070(void);

void func_001ADF00(void) {
    D_00810D38[0] = 0;
    *(unsigned char *)0x70003B93 = 0;
    func_001D2880();
    func_001D1EF0();
    func_001AEBA0(0xff);
    D_00275BDC = 1;
    func_001AB790(func_001AC070);
}
