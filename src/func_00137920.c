// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
extern int func_001B10B0(char *, int, int, int);
extern void bone_init_default_2(char *, int);
extern char *func_001AFA90(int);
extern int D_002753A8;
extern int D_0028A714[4];
extern unsigned char D_0081070A[16];
extern int func_00137EB0[16];

void func_00137920(char *arg0, char *arg1) {
    char *self;
    char *p;
    char *q;
    int cnt;

    self = arg0;
    cnt = *(unsigned char *)(arg0 + 4) + 1;
    *(unsigned char *)(arg0 + 4) = cnt;
    *(char *)(arg0 + 0) = 1;
    *(int *)(arg0 + 0x30) = (int)&D_002753A8;
    *(int *)(arg1 + 0x28) = 0x3F800000;
    *(short *)(arg1 + 0x32) = 0x3E8;
    *(unsigned char *)(arg0 + 0x5D) = 0x81;
    *(int *)(arg0 + 0x58) = D_0028A714[0];
    if (D_0081070A[0] != 0) {
        *(short *)(self + 0x34) = 0xBB8;
    } else {
        *(short *)(self + 0x34) = 0x7D0;
    }
    if (func_001B10B0(self, 0xA3, 0xA2, cnt) == 0) {
        bone_init_default_2(self, 0);
        *(int *)(self + 0xB0) = 0x42CDCEF3;
        *(int *)(self + 0xB4) = 0x420C0000;
        *(int *)(self + 0xB8) = 0x4321FC71;
        *(int *)(self + 0xBC) = 0x3F800000;
        p = func_001AFA90(8);
        if (p != 0) {
            *(char *)(p + 3) = 0;
            *(char *)(p + 0xD) = 0;
            *(int *)(p + 0x10) = (int)func_00137EB0;
            *(int *)(p + 0x20) = (int)self;
            q = func_001AFA90(8);
            if (q != 0) {
                *(char *)(q + 3) = 4;
                *(char *)(q + 0xD) = 1;
                *(int *)(q + 0x10) = (int)func_00137EB0;
                *(int *)(q + 0x20) = (int)self;
            }
        }
    }
}
