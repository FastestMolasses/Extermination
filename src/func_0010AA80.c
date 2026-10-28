// COMPILER: eegcc
// EEGCC_AS: 2.96
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.75% NEARMISS).
// Fixes read from the original instructions:
//  - the 2.96 gas fills the call delay slots with the following MMIO stores, as the
//    original does;
//  - func_001063E8 is declared returning void here: an unused int-returning call keeps
//    the return register live after the call and shifted the scratch-register choice
//    for the MMIO addresses and the 28-bit address mask.
//
// Behaviour: brackets a hardware transfer with func_00109B20(D_002412F4, buf) calls
// (buf[0] = 2 before, 3 after). Waits via func_001063E8, clears the register at
// 0x10002000, waits again, then programs DMA channel registers at 0x1000B400:
// address = a1 masked to 28 bits, count 4, control 0x101; then func_001063B8(a0) and a
// final wait.

extern int func_00109B20(int a0, void *a1);
extern void func_001063E8(void);
extern void func_001063B8(unsigned int a0);
extern int D_002412F4;

void func_0010AA80(int a0, int a1) {
    int local[8];

    local[0] = 2;
    func_00109B20(D_002412F4, local);
    func_001063E8();
    *(volatile int *)0x10002000 = 0;
    func_001063E8();
    *(volatile int *)0x1000B410 = a1 & 0xFFFFFFF;
    *(volatile int *)0x1000B420 = 4;
    *(volatile int *)0x1000B400 = 0x101;
    func_001063B8(a0);
    func_001063E8();
    local[0] = 3;
    func_00109B20(D_002412F4, local);
}
