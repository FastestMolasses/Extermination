// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
extern void bone_init_default_2(char *, int);
extern void func_00102948(char *, char *);
extern int func_001B10B0(char *, int, int);
extern int D_002753F8;
extern int D_0028A694[4];
extern unsigned char D_0081070A[16];
extern unsigned char D_00810808[16];

void func_001475A0(char *arg0, char *arg1) {
    *(unsigned char *)(arg0 + 4) = *(unsigned char *)(arg0 + 4) + 1;
    *(char *)(arg0 + 0) = 1;
    *(char *)(arg1 + 0x8A) = 1;
    *(int *)(arg1 + 0x40) = 0x3F800000;
    *(char *)(arg1 + 0x8B) = *(char *)(arg0 + 0xD);
    *(int **)(arg0 + 0x30) = &D_002753F8;
    *(char *)(arg1 + 0x83) = 0;
    func_00102948(arg1, arg0 + 0xB0);
    func_00102948(arg1 + 0x10, arg0 + 0xC0);
    *(int *)(arg0 + 0x58) = D_0028A694[0];
    if (D_00810808[0] == 0xFF) {
        *(unsigned char *)(arg0 + 0xD) |= 0x80;
    }
    if (*(unsigned char *)(arg0 + 0xD) & 0x80) {
        if (D_0081070A[0] != 0) {
            *(short *)(arg0 + 0x34) = 0x190;
        } else {
            *(short *)(arg0 + 0x34) = 0xFA;
        }
        *(int *)(arg0 + 0x60) = 0x3F933333;
        *(int *)(arg0 + 0x64) = 0x3F933333;
        *(int *)(arg0 + 0x68) = 0x3F933333;
        *(int *)(arg0 + 0x6C) = 0x3F800000;
        if (func_001B10B0(arg0, 0x83, 0x84) != 0) {
            return;
        }
    } else {
        if (D_0081070A[0] != 0) {
            *(short *)(arg0 + 0x34) = 0xFA;
        } else {
            *(short *)(arg0 + 0x34) = 0xB4;
        }
        if (func_001B10B0(arg0, 0x82, 0x84) != 0) {
            return;
        }
    }
    bone_init_default_2(arg0, 0);
}
