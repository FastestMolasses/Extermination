// COMPILER: eegcc
// CFLAGS: -O2
extern unsigned char D_0027CCC0[];
extern unsigned char D_0027E0C0[];

int func_00119890(int a0, int a1) {
    unsigned char *q = D_0027E0C0;
    unsigned short t;
    int ret = 0;

    switch (a0) {
    case 0:
        if (*(unsigned short *)(D_0027CCC0 + a1 * 0x6A) == 1) {
            switch (*(unsigned short *)(D_0027CCC0 + a1 * 0x6A + 0x1A)) {
            case 1:
                ret = 1;
                break;
            case 2:
                ret = 2;
                break;
            case 3:
                ret = 4;
                break;
            }
        }
        break;
    case 1:
        q += a1 * 0x78;
        t = *(unsigned short *)(q + 0x2E);
        if (t == 1) {
            if (*(unsigned short *)(q + 0x32) == 0) {
                ret = 1;
            }
        }
        if (t == 0) {
            if (*(unsigned short *)(q + 0x32) == 1) {
                ret = 2;
            }
        }
        break;
    }
    return ret;
}
