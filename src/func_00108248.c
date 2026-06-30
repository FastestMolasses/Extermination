// COMPILER: eegcc
// CFLAGS: -O2
extern int D_00241220;
extern int D_00241228[4];

int func_00108248(void) {
    *(volatile int *)0x1000E010 = 8;
    if (*(volatile int *)0x1000B020 != 0 ||
        (*(volatile int *)0x1000B000 & 0x100) != 0) {
        D_00241220 = 1;
        return 0;
    }
    if (D_00241228[0] == 0) {
        *(volatile int *)0x1000B010 = D_00241228[1];
        *(volatile int *)0x1000B020 = D_00241228[2];
        *(volatile int *)0x1000B000 = 0x100;
        *(volatile int *)0x10002000 = D_00241228[3] | 0x70000000;
    }
    *(volatile int *)&D_00241228[0] = *(volatile int *)&D_00241228[0] + 1;
    __asm__ __volatile__("sync\n\tei" ::: "memory");
    return 0;
}
