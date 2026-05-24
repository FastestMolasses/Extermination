// CFLAGS: -O4,p -sdatathreshold 0
extern unsigned char D_00810CB8[];
extern char D_008106B0;
extern char D_008106B1;

int func_001C4720(int a0, int a1) {
    D_00810CB8[a0] = D_00810CB8[a0] + a1;
    D_008106B0 = 2;
    D_008106B1 = a0;
    return 0;
}
