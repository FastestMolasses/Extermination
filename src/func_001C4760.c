// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// The two status-byte stores must stay in program order (const 3 to D_008106B0
// first, then a0 to D_008106B1). volatile prevents 991202's store reorder while
// keeping both as distinct symbols; mwcc 2.3.3 then matches. Plain C. 100.0 on 233.
extern unsigned char D_00810CC3[];
extern volatile unsigned char D_008106B0;
extern volatile unsigned char D_008106B1;

int func_001C4760(int a0, int a1) {
    D_00810CC3[a0] += a1;
    if (a0 >= 0x20) {
        D_008106B0 = 3;
        D_008106B1 = a0;
    }
    return 0;
}
