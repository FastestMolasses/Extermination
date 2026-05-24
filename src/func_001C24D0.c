// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001026A0(int, int, int);
extern void func_001028B8(int, int, int);
extern void func_0019AD00(int, int, int);
extern int D_700038C0;

void func_001C24D0(int a0, int a1, int a2) {
    func_001026A0((int)&D_700038C0, a2, a1);
    func_001028B8((int)&D_700038C0, (int)&D_700038C0, a0 + 0xB0);
    func_0019AD00(a0, (int)&D_700038C0, 0x80000006);
}
