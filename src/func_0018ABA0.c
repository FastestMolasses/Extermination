// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Per-state entity behavior driver: state 0 seeds/copies the 0xA0 transform block and arms the timer; state 1 emits the keyed sound/effect events and ticks the timer down; states 2/3 delegate to func_001AFC10.
extern void func_00102948(char *a, char *b);
extern int func_00122BB8(void);
extern void func_001AFC10(char *p);
extern float func_001B1470(float a);
extern void func_001B17A0(char *p);
extern void func_001EFD20(int a, char *b);
extern void func_001EFD90(int a, char *b, char *c);
extern void func_001F00A0(int a, char *b, char *c, int d);
extern void func_001FBD50(char *p, int a, int b, float f);
extern char D_002754E0;
extern float D_00810374[3];

void func_0018ABA0(char *arg0) {
    unsigned char st;
    unsigned short flg;
    short t;

    st = *(unsigned char *)(arg0 + 4);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 0xA) = 0;
        if (*(float *)(arg0 + 0xCC) == 0.0f) {
            *(int *)(arg0 + 0xA0) = 0;
            *(float *)(arg0 + 0xA4) = func_001B1470(3.1415927f + D_00810374[0]);
            *(int *)(arg0 + 0xA8) = 0;
            *(int *)(arg0 + 0xAC) = 0x3F800000;
        } else {
            func_00102948(arg0 + 0xA0, arg0 + 0xC0);
        }
        if (*(unsigned char *)(arg0 + 0xD) == 2) {
            *(int *)(arg0 + 0xAC) = 0;
        } else {
            *(int *)(arg0 + 0xAC) = 0x3F800000;
        }
        *(unsigned char *)(arg0 + 4) = *(unsigned char *)(arg0 + 4) + 1;
        *(short *)(arg0 + 0x36) = 1;
        *(short *)(arg0 + 0x28) = 4;
        *(char *)(arg0 + 0) = 1;
        *(char **)(arg0 + 0x30) = &D_002754E0;
        break;
    case 1:
        if (*(unsigned char *)(arg0 + 5) == 1 && *(unsigned char *)(arg0 + 0xD) != 0) {
            if (*(unsigned char *)(arg0 + 0xA) != 0) {
                func_001EFD90(0x80000026, arg0 + 0xB0, arg0 + 0xA0);
            } else {
                flg = *(unsigned short *)(arg0 + 0x2E);
                if ((flg & 0x300) == 0) {
                    func_001EFD20(0x80000019, arg0 + 0xB0);
                    func_001F00A0(0x80000060, arg0 + 0xB0, arg0 + 0xA0, *(unsigned short *)(arg0 + 0x2E) & 1);
                    if (*(unsigned char *)(arg0 + 0xD) != 2) {
                        if (*(unsigned short *)(arg0 + 0x2E) & 0x10) {
                            if ((func_00122BB8() >> 12) & 1) {
                                func_001FBD50(arg0, 0x18A, 0, 300.0f);
                            } else {
                                func_001FBD50(arg0, 0x18B, 0, 300.0f);
                            }
                        } else if ((func_00122BB8() >> 12) & 1) {
                            func_001FBD50(arg0, 0x188, 0, 300.0f);
                        } else {
                            func_001FBD50(arg0, 0x189, 0, 300.0f);
                        }
                    }
                } else if (flg & 0x100) {
                    func_001F00A0(0x80000003, arg0 + 0xB0, arg0 + 0xA0, flg & 1);
                    if (*(unsigned char *)(arg0 + 0xD) != 2) {
                        if ((func_00122BB8() >> 12) & 1) {
                            func_001FBD50(arg0, 0x188, 0, 300.0f);
                        } else {
                            func_001FBD50(arg0, 0x189, 0, 300.0f);
                        }
                    }
                } else {
                    func_001F00A0(0x80000060, arg0 + 0xB0, arg0 + 0xA0, flg & 1);
                    if (*(unsigned char *)(arg0 + 0xD) != 2) {
                        if ((func_00122BB8() >> 12) & 1) {
                            func_001FBD50(arg0, 0x18A, 0, 300.0f);
                        } else {
                            func_001FBD50(arg0, 0x18B, 0, 300.0f);
                        }
                    }
                }
            }
        }
        *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
        t = *(short *)(arg0 + 0x28);
        *(short *)(arg0 + 0x28) = t - 1;
        if (t == 0) {
            *(char *)(arg0 + 0) = 2;
            *(unsigned char *)(arg0 + 4) = *(unsigned char *)(arg0 + 4) + 1;
        }
        *(unsigned char *)(arg0 + 0xA) = 0;
        func_001B17A0(arg0);
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
