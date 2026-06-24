// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// 3-way dispatch on the byte at p[3]: 0 -> func_001C47A0, 1 -> func_001C4720,
// else -> func_001C4760, each called with the halfword at p+0x2E and constant 1.
// Always returns 1. mwcc233/24 reproduce CW's bnez/bne branch lowering.
extern int func_001C4720(int, int);
extern int func_001C4760(int, int);
extern int func_001C47A0(int, int);

int func_001B6EA0(unsigned char *p) {
    if (p[3] == 0) {
        func_001C47A0(*(unsigned short *)(p + 0x2E), 1);
    } else if (p[3] == 1) {
        func_001C4720(*(unsigned short *)(p + 0x2E), 1);
    } else {
        func_001C4760(*(unsigned short *)(p + 0x2E), 1);
    }
    return 1;
}
