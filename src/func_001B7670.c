// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Script command: argument +8 of the command record is 0 or 1. 0 sets the
// byte 0x70003B91 from 1 to 0, 1 sets it from 0 to 1 (other values of the byte
// are left alone). Returns 1.
extern unsigned char D_70003B91;
int func_001B7670(int a0, int a1, char *cmd) {
    switch (*(int *)(cmd + 8)) {
    case 0:
        if (D_70003B91 == 1) D_70003B91 = 0;
        break;
    case 1:
        if (D_70003B91 == 0) D_70003B91 = 1;
        break;
    }
    return 1;
}
