// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001026A0(int, int, int);
extern void func_001028B8(int, int, int);
extern void func_0019B4C0(int, int, int, int);
extern int D_700038C0;
extern int D_700038D0;

void func_001C2540(int a0, int a1, int a2, int a3) {
    func_001026A0((int)&D_700038C0, a3, a1);
    func_001028B8((int)&D_700038C0, (int)&D_700038C0, a0 + 0xB0);
    func_001026A0((int)&D_700038D0, a3, a2);
    func_0019B4C0(a0, (int)&D_700038C0, (int)&D_700038D0, 0x80000006);
}
