// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-state driver for the enemy at arg0: dispatches on the state byte arg0+6
// (0,1,2,3,10..13), then always runs the shared tail (func_001764E0 / func_00179880 /
// func_00175900). State 1 is the stagger/knockback/death-transition logic.
extern void func_001749A0(unsigned char *e, int clip, int flags, float blend);
extern void func_00174AC0(unsigned char *e, int f);
extern int func_001754E0(unsigned char *e, int f);
extern int func_00175900(unsigned char *e, int f);
extern void func_001764E0(unsigned char *e);
extern void func_00179880(unsigned char *e, void *p);
extern void func_0017C540(unsigned char *e);
extern void func_001B61C0(int a, int b, int c, int d);
extern void func_001FBD50(unsigned char *p, int a, int b, float f);
extern void func_0021C120(unsigned char *e);
extern int func_0021C190(unsigned char *e);
extern void func_0021C270(unsigned char *e);
extern void func_0021C350(unsigned char *e);
extern unsigned char D_008106BC;
extern unsigned char D_008106F1;
extern unsigned char D_0081083C;

void func_0021F330(unsigned char *arg0) {
    unsigned char st;
    signed char c;
    float zf;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(unsigned char *)(arg0 + 7) = 0;
        func_001749A0(arg0, 0x2C, 0, 0.0f);
        func_00174AC0(arg0, 0);
        *(float *)(arg0 + 0x26C) = *(float *)(arg0 + 0x24C);
        *(short *)(arg0 + 0x28) = 0;
        *(signed char *)(arg0 + 0x2FE) = 0;
        *(int *)(arg0 + 0x2EC) = 0;
        break;
    case 1:
        c = *(signed char *)(arg0 + 0x2FE);
        *(signed char *)(arg0 + 0x2FE) = c - 1;
        if (c == 0) {
            *(signed char *)(arg0 + 0x2FE) = 0;
            *(short *)(arg0 + 0x28) = 0;
        }
        func_00174AC0(arg0, 0);
        if (func_001754E0(arg0, 6) != 0 || *(unsigned char *)0x70003B8D != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(unsigned char *)(arg0 + 0) = *(unsigned char *)(arg0 + 0) | 4;
            func_001749A0(arg0, 0x2D, 0, 8.0f);
            func_001FBD50(arg0, 0x15A, 0, 300.0f);
            *(int *)(arg0 + 0x224) = 0;
            goto state1_done;
        }
        if (D_0081083C == 0) {
            D_008106BC = 0;
            *(unsigned char *)(arg0 + 0) = 3;
            *(short *)(arg0 + 0x20E) = 0x3C;
            *(signed char *)(arg0 + 0x25C) = 0;
            func_0017C540(arg0);
            goto state1_done;
        }
        if (*(float *)(arg0 + 0x224) != (zf = 0.0f)) {
            func_0021C350(arg0);
            if (*(float *)(arg0 + 0x220) <= 0.0f) {
                *(unsigned char *)(arg0 + 0) = 2;
                if (*(unsigned char *)(arg0 + 0x234) == 1) {
                    *(signed char *)(arg0 + 4) = 2;
                    *(signed char *)(arg0 + 5) = 3;
                    *(unsigned char *)(arg0 + 6) = 0;
                    *(signed char *)(arg0 + 0x1F0) = 0x3F;
                } else {
                    *(signed char *)(arg0 + 4) = 2;
                    *(signed char *)(arg0 + 5) = 1;
                    *(unsigned char *)(arg0 + 6) = 0;
                    *(signed char *)(arg0 + 0x1F0) = 0x40;
                }
                return;
            }
            func_001B61C0(0, 0xC0, 5, 1);
            func_001FBD50(arg0, 0x154, 0, 300.0f);
            func_001749A0(arg0, 0x1E, 1, 1.0f);
            *(unsigned char *)(arg0 + 7) = 1;
        } else if (*(float *)(arg0 + 0x22C) != zf) {
            func_0021C270(arg0);
            if (!(*(float *)(arg0 + 0x228) < 100.0f) && D_008106F1 != 0) {
                *(unsigned char *)(arg0 + 6) = 0xA;
                D_008106BC = 1;
            } else {
                func_001B61C0(0, 0xC0, 5, 1);
                func_001FBD50(arg0, 0x154, 0, 300.0f);
                func_001749A0(arg0, 0x1E, 1, 1.0f);
                *(unsigned char *)(arg0 + 7) = 1;
            }
        }
        switch (*(unsigned char *)(arg0 + 7)) {
        case 0:
            if (*(short *)(arg0 + 0x28) != 0) {
                *(int *)(arg0 + 0x204) = 0x400CCCCD;
            } else {
                *(int *)(arg0 + 0x204) = 0x3F000000;
            }
            break;
        case 1:
            if (*(int *)(arg0 + 0x200) & 0x1000) {
                func_001749A0(arg0, 0x2C, 0, 4.0f);
                *(unsigned char *)(arg0 + 7) = 0;
            } else {
                *(int *)(arg0 + 0x204) = 0x3F99999A;
            }
            break;
        }
    state1_done:
        *(float *)(arg0 + 0x26C) = *(float *)(arg0 + 0x24C);
        break;
    case 2:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            D_008106BC = 1;
        }
        break;
    case 3:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            D_008106BC = 0;
            *(unsigned char *)(arg0 + 0) = 3;
            *(short *)(arg0 + 0x20E) = 0x3C;
            *(signed char *)(arg0 + 0x25C) = 0;
            func_0017C540(arg0);
            *(int *)(arg0 + 0x224) = 0;
            *(int *)(arg0 + 0x22C) = 0;
        }
        break;
    case 10:
        *(unsigned char *)(arg0 + 6) = st + 1;
        func_001749A0(arg0, 0x1C7, 0, 1.0f);
        break;
    case 11:
        if (*(float *)(arg0 + 0x3C) <= 60.0f) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_0021C120(arg0);
        }
        break;
    case 12:
        if (func_0021C190(arg0) != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        } else {
            *(int *)(arg0 + 0x204) = 0x3DCCCCCD;
        }
        break;
    case 13:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            D_008106BC = 0;
            *(unsigned char *)(arg0 + 0) = 3;
            *(short *)(arg0 + 0x20E) = 0x3C;
            *(signed char *)(arg0 + 0x25C) = 0;
            func_0017C540(arg0);
            *(int *)(arg0 + 0x224) = 0;
            *(int *)(arg0 + 0x22C) = 0;
        } else {
            *(int *)(arg0 + 0x204) = 0x3F000000;
        }
        break;
    }
    func_001764E0(arg0);
    if (*(unsigned char *)(arg0 + 0xF) != 0) {
        D_008106BC = 1;
    }
    func_00179880(arg0, arg0 + 0x2EC);
    func_00175900(arg0, 1);
}
