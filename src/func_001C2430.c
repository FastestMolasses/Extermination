// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001026A0(int, int, int);
extern void func_001028B8(int, int, int);
extern void func_0019AB20(int, int, int, int);
extern int D_700038C0;
extern int D_700038D0;

void func_001C2430(int a0, int a1, int a2) {
    func_001026A0((int)&D_700038C0, a2, a1);
    func_001028B8((int)&D_700038C0, (int)&D_700038C0, a0 + 0xB0);
    *(volatile int *)(0x700038D0) = 0;
    *(volatile int *)(0x700038D4) = 0xC1200000;
    *(volatile int *)(0x700038D8) = 0;
    *(volatile int *)(0x700038DC) = 0;
    func_0019AB20(a0, (int)&D_700038C0, (int)&D_700038D0, 0x80000007);
}
