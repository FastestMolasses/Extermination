// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// State-machine update dispatch on actor state byte p[6]; advances animation/state
// (func_001749A0/00174AC0), runs timers, and on state-2 evaluates float gates at
// p[0x220]/p[0x224] to branch into death/recovery substates.
void func_001749A0(char *, int, int, float);
void func_00174AC0(char *, int);
int func_001754E0(char *, int);
void func_00175900(char *, int);
void func_00179880(char *, char *);
void func_0017C540(void);
void func_001B61C0(int, int, int, int);
void func_001FBD50(char *, int, int, float);
void func_0021C350();
extern unsigned char D_008106BD;

void func_00221C70(char *arg0) {
    unsigned char st;
    signed char dec;
    signed char v;
    unsigned char sub;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = (unsigned char)(st + 1);
        func_001749A0(arg0, 0x33, 0, 0.0f);
        break;
    case 1:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            func_0021C350();
            if (*(float *)(arg0 + 0x220) <= 0.0f) {
                if (*(unsigned char *)(arg0 + 0x234) == 1) {
                    *(signed char *)(arg0 + 4) = 2;
                    *(signed char *)(arg0 + 5) = 3;
                    v = 0x3F;
                    *(unsigned char *)(arg0 + 6) = 0;
                    *(signed char *)(arg0 + 0x1F0) = v;
                } else {
                    *(signed char *)(arg0 + 4) = 2;
                    *(signed char *)(arg0 + 5) = 1;
                    v = 0x40;
                    *(unsigned char *)(arg0 + 6) = 0;
                    *(signed char *)(arg0 + 0x1F0) = v;
                }
                return;
            }
            *(unsigned char *)(arg0 + 6) = (unsigned char)(*(unsigned char *)(arg0 + 6) + 1);
            *(unsigned char *)(arg0 + 7) = 0;
            func_001749A0(arg0, 0x34, 0, 0.0f);
            *(short *)(arg0 + 0x28) = 0;
            *(signed char *)(arg0 + 0x2FE) = 0;
            func_00174AC0(arg0, 0);
            *(float *)(arg0 + 0x26C) = *(float *)(arg0 + 0x24C);
            *(int *)(arg0 + 0x2EC) = 0;
        }
        break;
    case 2:
        dec = *(signed char *)(arg0 + 0x2FE);
        *(signed char *)(arg0 + 0x2FE) = (signed char)(dec - 1);
        if (dec == 0) {
            *(signed char *)(arg0 + 0x2FE) = 0;
            *(short *)(arg0 + 0x28) = 0;
        }
        func_00174AC0(arg0, 0);
        if ((func_001754E0(arg0, 8) != 0) || (*(unsigned char *)0x70003B8D != 0)) {
            *(unsigned char *)(arg0 + 6) = (unsigned char)(*(unsigned char *)(arg0 + 6) + 1);
            func_001749A0(arg0, 0x38, 0, 4.0f);
            D_008106BD = 1;
            *(float *)(arg0 + 0x224) = 0.0f;
            break;
        }
        *(float *)(arg0 + 0x26C) = *(float *)(arg0 + 0x24C);
        if (*(float *)(arg0 + 0x224)) {
            func_0021C350(arg0);
            if (*(float *)(arg0 + 0x220) <= 0.0f) {
                if (*(unsigned char *)(arg0 + 0x234) == 1) {
                    *(signed char *)(arg0 + 4) = 2;
                    *(signed char *)(arg0 + 5) = 3;
                    v = 0x3F;
                    *(unsigned char *)(arg0 + 6) = 0;
                    *(signed char *)(arg0 + 0x1F0) = v;
                } else {
                    *(signed char *)(arg0 + 4) = 2;
                    *(signed char *)(arg0 + 5) = 1;
                    v = 0x40;
                    *(unsigned char *)(arg0 + 6) = 0;
                    *(signed char *)(arg0 + 0x1F0) = v;
                }
                return;
            }
            func_001B61C0(0, 0xC0, 5, 1);
            func_001FBD50(arg0, 0x154, 0, 300.0f);
            func_001749A0(arg0, 0x1CA, 0, 0.0f);
            *(unsigned char *)(arg0 + 7) = 1;
        }
        sub = *(unsigned char *)(arg0 + 7);
        switch (sub) {
        case 0:
            if (*(short *)(arg0 + 0x28) != 0) {
                *(int *)(arg0 + 0x204) = 0x40000000;
            } else {
                *(int *)(arg0 + 0x204) = 0x3F000000;
            }
            break;
        case 1:
            if (*(int *)(arg0 + 0x200) & 0x1000) {
                func_001749A0(arg0, 0x34, 0, 1.0f);
                *(unsigned char *)(arg0 + 7) = 0;
            }
            break;
        }
        break;
    case 3:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(short *)(arg0 + 0x20E) = 0x3C;
            *(signed char *)(arg0 + 0x25C) = 0;
            func_0017C540();
        }
        break;
    }
    func_00179880(arg0, arg0 + 0x2EC);
    func_00175900(arg0, 1);
}
