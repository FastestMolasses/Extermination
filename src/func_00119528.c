// COMPILER: eegcc
// CFLAGS: -O2
typedef struct {
    int f0;
    int f4;
    int f8;
} Slot;
typedef struct {
    char _pad[0xC];
    int magic;
} Hdr;
extern Slot D_0027C6C0[];
int func_00119528(Hdr *a0, unsigned int a1, int a2) {
    int i;
    int ret = -1;
    if (a0->magic != 0x64685353) {
        return -1;
    }
    for (i = 0; i < 0x7F; i++) {
        if (D_0027C6C0[i].f0 == 0) {
            D_0027C6C0[i].f0 = 1;
            D_0027C6C0[i].f4 = (int)a0;
            D_0027C6C0[i].f8 = a1 >> 3;
            ret = i;
            break;
        }
    }
    return ret;
}
