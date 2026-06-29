// COMPILER: eegcc
// CFLAGS: -O2
extern unsigned int func_00106948(int);

extern int D_0024145C;
extern int D_00241460;
extern int D_00241464;
extern int D_00241468;
extern int D_0024146C;
extern int D_00241470;
extern int D_00241474;

void func_0010ACA8(void) {
    D_0024145C = func_00106948(3);
    D_00241460 = func_00106948(1);
    if (D_00241460 != 0) {
        D_00241464 = func_00106948(8);
        D_00241468 = func_00106948(8);
        D_0024146C = func_00106948(8);
    }
    D_00241470 = func_00106948(0xE);
    func_00106948(1);
    D_00241474 = func_00106948(0xE);
}
