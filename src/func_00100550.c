// COMPILER: eegcc
// CFLAGS: -O2
// Push a 5-qword GS register block to the GIF FIFO window; the layout
// depends on whether the active context (func_00100268()->f6) is 1.
extern short *func_00100268(void);

void func_00100550(long *a0) {
    if (func_00100268()[3] == 1) {
        *(volatile long *)0x12000000 = a0[0];
        *(volatile long *)0x12000070 = a0[2];
        *(volatile long *)0x12000080 = a0[3];
        *(volatile long *)0x120000C0 = a0[4];
    } else {
        *(volatile long *)0x12000000 = a0[0];
        *(volatile long *)0x12000020 = a0[1];
        *(volatile long *)0x12000090 = a0[2];
        *(volatile long *)0x120000A0 = a0[3];
        *(volatile long *)0x120000E0 = a0[4];
    }
}
