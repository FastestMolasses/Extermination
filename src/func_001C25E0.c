// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001026A0(int, int, int);
extern void func_001028B8(int, int, int);
extern void func_0019B4C0(int, int, int, int);
extern int D_700038B0;
extern int D_700038C0;
extern int D_700038D0;

void func_001C25E0(int a0, int a1) {
    *(volatile int *)(0x700038B0) = 0;
    *(volatile int *)(0x700038B4) = 0xC0800000;
    *(volatile int *)(0x700038B8) = 0;
    *(volatile int *)(0x700038BC) = 0x3F800000;
    func_001026A0((int)&D_700038C0, a0 + 0xD0, a1);
    func_001028B8((int)&D_700038C0, (int)&D_700038C0, a0 + 0xB0);
    func_001026A0((int)&D_700038D0, a0 + 0xD0, (int)&D_700038B0);
    func_0019B4C0(a0, (int)&D_700038C0, (int)&D_700038D0, 6);
}
