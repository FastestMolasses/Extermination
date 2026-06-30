// COMPILER: eegcc
// CFLAGS: -O2
extern int D_00241C68;
extern int D_00276C98;
struct C788 { int flag; int value; };
extern struct C788 D_00276C90;
extern int _iWakeupThread(void);
extern void _iSignalSema(int sema);

int func_0010C710(int a0) {
    int s0;
    __asm__ __volatile__("addiu $3, $0, -0x2F\n\tsyscall 0\n\tdaddu %0, $2, $0"
                         : "=r"(s0) :: "$2", "$3", "memory");
    if (s0 != a0) {
        return _iWakeupThread();
    }
    if ((unsigned int)s0 >= 0x100) {
        return -1;
    }
    if (D_00241C68 == 0) {
        return -1;
    }
    D_00276C90.flag = 0;
    D_00276C90.value = s0;
    _iSignalSema(D_00276C98);
    return s0;
}
