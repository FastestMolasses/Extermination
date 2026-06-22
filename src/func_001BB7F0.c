// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
extern char **D_00275B40;
extern unsigned char D_008106B8[];

int func_001BB7F0(char *arg0) {
    if (D_008106B8[0] == 0) {
        unsigned char t = *(unsigned char *)(arg0 + 3);
        if (t == 8 || t == 0x16) {
            *(int *)(D_00275B40[0] + 0x7C) = 0;
        } else {
            *(int *)(D_00275B40[1] + 0x7C) = 0;
            *(int *)(D_00275B40[2] + 0x7C) = 0;
        }
        *(char *)(arg0 + 0xB) = 0;
        return 1;
    }
    return 0;
}
