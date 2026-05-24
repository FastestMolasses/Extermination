// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001C3BE0(int);
extern void func_00102958(int, int);
extern void func_001031E0(int, int);
extern int D_70003000;
extern int D_70003030;

void func_001C3D60(int a0) {
    func_001C3BE0(a0);
    func_00102958(a0 + 0xD0, (int)&D_70003000);
    func_001031E0((int)&D_70003030, a0 + 0xB0);
}
