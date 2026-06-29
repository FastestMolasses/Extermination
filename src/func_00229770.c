// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001FE8D0(void);
extern int func_001FE9A0(int, char *, char *);
extern int func_001FECB0(int, int, char *, char *, char *);

int func_00229770(char *arg0) {
    int a0;
    int v1b;

    if (func_001FE9A0(1, arg0 + 0x50, arg0 + 0x54) == 0) {
        return 0;
    }
    switch (*(unsigned char *)(arg0 + 2)) {
    case 0:
        if (func_001FECB0(0, 0, arg0 + 0x58, arg0 + 0x5C, arg0 + 0x60) != -1) {
            *(int *)(arg0 + 0x30) = *(int *)(arg0 + 0x58);
            *(int *)(arg0 + 0x38) = *(int *)(arg0 + 0x5C);
            *(int *)(arg0 + 0x40) = *(int *)(arg0 + 0x60);
            *(unsigned char *)(arg0 + 2) = 1;
            func_001FE8D0();
            if (*(int *)(arg0 + 0x58) == 2) {
                *(int *)(arg0 + 0x28) = 1;
            }
        }
        goto ret0;
    case 1:
        if (func_001FECB0(1, 0, arg0 + 0x58, arg0 + 0x5C, arg0 + 0x60) != -1) {
            *(int *)(arg0 + 0x34) = *(int *)(arg0 + 0x58);
            *(int *)(arg0 + 0x3C) = *(int *)(arg0 + 0x5C);
            *(int *)(arg0 + 0x44) = *(int *)(arg0 + 0x60);
            func_001FE8D0();
            if (*(int *)(arg0 + 0x58) == 2) {
                *(int *)(arg0 + 0x2C) = 1;
            }
            *(unsigned char *)(arg0 + 2) = 2;
        }
        goto ret0;
    case 2:
        v1b = *(int *)(arg0 + 0x30);
        if (v1b == 2 && *(int *)(arg0 + 0x40) == 0) {
            *(signed char *)(arg0 + 0) = 3;
            return 1;
        }
        a0 = *(int *)(arg0 + 0x34);
        if (a0 == 2 && *(int *)(arg0 + 0x44) == 0) {
            *(signed char *)(arg0 + 0) = 3;
            return 1;
        }
        if (v1b == 2 && *(int *)(arg0 + 0x40) == 1) {
            *(signed char *)(arg0 + 1) = 1;
            *(unsigned char *)(arg0 + 2) = 0;
            return 1;
        }
        if (a0 == 2 && *(int *)(arg0 + 0x44) == 1) {
            *(signed char *)(arg0 + 1) = 1;
            *(unsigned char *)(arg0 + 2) = 0;
            return 1;
        }
        *(signed char *)(arg0 + 1) = 2;
        *(unsigned char *)(arg0 + 2) = 0;
        return 1;
    }
ret0:
    return 0;
}
