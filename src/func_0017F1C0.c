// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001026A0(int, int, int);
extern void func_0019AD00(int, int, int);
extern int D_700038A0;
extern int D_700038B0;

void func_0017F1C0(int a0) {
    *(volatile int *)(0x700038A0) = 0;
    *(volatile int *)(0x700038A4) = 0x41200000;
    *(volatile int *)(0x700038A8) = 0x40A00000;
    *(volatile int *)(0x700038AC) = 0x3F800000;
    func_001026A0((int)&D_700038B0, a0 + 0xD0, (int)&D_700038A0);
    func_0019AD00(a0, (int)&D_700038B0, 7);
}
