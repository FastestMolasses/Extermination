// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001029C0(int);
extern void func_00102C58(int, int, int);
extern void func_001026A0(int, int, int);
extern int D_700036A0;

void func_00131F20(int a0, int a1, int a2) {
    func_001029C0((int)&D_700036A0);
    func_00102C58((int)&D_700036A0, (int)&D_700036A0, a0 + 0xC0);
    func_001026A0(a2, (int)&D_700036A0, a1);
}
