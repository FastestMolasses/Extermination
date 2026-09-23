// COMPILER: eegcc
// CFLAGS: -O2
//
// MATCH NOTE (m3-matching lane): 96.67% under mwcc 991202 -> 100%;
// SDK-region code built with ee-gcc (as the rest of the lowmem SDK). Source unchanged apart from the compiler line.

extern int func_0011FD88(void);
typedef int (*fn_t)(void);
extern void func_001209B0(int, fn_t);

void func_00120038(int a0) {
    func_001209B0(a0, func_0011FD88);
}
