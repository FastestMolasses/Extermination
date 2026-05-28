// CFLAGS: -O4,p -sdatathreshold 0
extern void sub_PsIIlibpad_2000(int);
extern void func_001B5860(int, int, int, int, int);
extern int D_00810E70;
extern int D_00810E40;
extern int D_00810D40;
extern short D_00810E7A;

void func_001B5790(void) {
    sub_PsIIlibpad_2000(0);
    func_001B5860((int)&D_00810E70, (int)&D_00810E40, 0, 0, (int)&D_00810D40);
    D_00810E7A = 0x20;
}
