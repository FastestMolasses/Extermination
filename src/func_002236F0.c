// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Player/actor state-machine tick: dispatches on the actor state byte at +6 and drives
// per-state transitions, timers, camera-shake/effect calls and the 0xB4 accumulator.

extern int func_00122BB8(void);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001749A0(unsigned char *e, int clip, int flags, float blend);
extern int func_00175900(unsigned char *e, int f);
extern void func_00178B90(unsigned char *e, int f);
extern void func_00179880(unsigned char *e, void *p);
extern void func_00182870(unsigned char *e, int f);
extern int func_00188610(unsigned char *e);
extern void func_001B61C0(int a, int b, int c, int d);
extern void func_001FBD50(unsigned char *e, int a, int b, float f);
extern void func_0021C120(unsigned char *e);
extern int func_0021C190(unsigned char *e);
extern void func_0021C270(unsigned char *e);
extern void func_0021C350(unsigned char *e);
extern void func_0021D250(unsigned char *e, int b);
extern void func_0021D2E0(unsigned char *e, int a, int b);

extern int **D_00275B40;
extern unsigned char D_008106F1[];
extern int D_700038A0[];

void func_002236F0(unsigned char *arg0) {
    unsigned char st;
    int flag;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        func_001B61C0(0, 0xC0, 5, 1);
        *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        *(char *)(arg0 + 7) = 0;
        flag = func_00122BB8() & 1;
        if (*(unsigned char *)(arg0 + 0xF) & 2) {
            flag = 1;
            *(unsigned char *)(arg0 + 0xF) = 0;
        }
        if (*(float *)(arg0 + 0x224)) {
            func_001FBD50(arg0, 0x152, 0, 300.0f);
            func_0021C350(arg0);
        }
        if (*(float *)(arg0 + 0x22C)) {
            func_001FBD50(arg0, 0x153, 0, 300.0f);
            func_0021C270(arg0);
        }
        if (*(float *)(arg0 + 0x220) <= 0.0f) {
            *(unsigned char *)(arg0 + 6) = 0xA;
            *(float *)(arg0 + 0x21C) = 0.0f;
            *(float *)(arg0 + 0x2E4) = 0.0f;
            break;
        }
        if (!(*(float *)(arg0 + 0x228) < 100.0f) && D_008106F1[0] != 0) {
            *(unsigned char *)(arg0 + 6) = 0x14;
            flag = 0;
        }
        if (flag == 0) {
            func_001749A0(arg0, 0x14F, 0, 8.0f);
            break;
        }
        func_001749A0(arg0, 0x150, 0, 8.0f);
        break;
    case 1:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(char *)(arg0 + 4) = 1;
            *(char *)(arg0 + 5) = 0x1B;
            *(unsigned char *)(arg0 + 6) = 0xA;
            *(char *)(arg0 + 7) = 0;
            *(char *)(arg0 + 0x1F0) = 0x2F;
            *(char *)(arg0 + 0x1F1) = 1;
            *(short *)(arg0 + 0x20E) = 0x3C;
            func_001749A0(arg0, func_00188610(arg0), 0, 16.0f);
        }
        break;
    case 0xA:
        *(unsigned char *)(arg0 + 6) = st + 1;
        func_001749A0(arg0, 0x151, 0, 8.0f);
        break;
    case 0xB:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(unsigned char *)(arg0 + 6) = st + 1;
        }
        break;
    case 0xC:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(int *)0x700038A0 = 0;
            *(int *)0x700038A4 = 0xC0800000;
            *(int *)0x700038A8 = 0x40A00000;
            *(int *)0x700038AC = 0x3F800000;
            func_001026A0(arg0 + 0xB0, arg0 + 0xD0, D_700038A0);
            *(float *)(arg0 + 0xB4) += -0.2f;
            func_001749A0(arg0, 0x72, 0, 1.0f);
            *(char *)(arg0 + 0x1F0) = 0xD;
            break;
        }
        *(float *)(arg0 + 0x38) = *(float *)((char *)*D_00275B40 + 8) - *(float *)(arg0 + 0x21C);
        *(float *)(arg0 + 0x21C) = *(float *)((char *)*D_00275B40 + 8);
        func_00178B90(arg0, 1);
        *(float *)(arg0 + 0x2EC) = *(float *)((char *)*D_00275B40 + 4) - *(float *)(arg0 + 0x2E4);
        *(float *)(arg0 + 0x2E4) = *(float *)((char *)*D_00275B40 + 4);
        *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0x2EC);
        break;
    case 0xD:
        func_00179880(arg0, arg0 + 0x2EC);
        if (func_00175900(arg0, 1) != 0) {
            func_00182870(arg0, 1);
            func_001FBD50(arg0, 0x156, 0, 300.0f);
            if (*(unsigned char *)(arg0 + 0xF) == 0x63 ||
                *(unsigned char *)(arg0 + 0x234) == 1) {
                *(char *)(arg0 + 4) = 2;
                *(char *)(arg0 + 5) = 3;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 0x1F0) = 0x3F;
                break;
            }
            func_001749A0(arg0, 0x2B, 0, 1.0f);
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(char *)(arg0 + 7) = 0;
            *(char *)(arg0 + 0x1F0) = 0x40;
            func_001B61C0(1, 0xEE, 0x3C, 1);
            break;
        }
        if (*(unsigned char *)(arg0 + 0x23A) == 0x5D) {
            func_0021D250(arg0, 0);
        }
        break;
    case 0xE:
        func_0021D2E0(arg0, 0x78, 0);
        break;
    case 0x14:
        if (*(float *)(arg0 + 0x3C) <= 21.0f) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_0021C120(arg0);
        }
        break;
    case 0x15:
        if (func_0021C190(arg0) != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        } else {
            *(int *)(arg0 + 0x204) = 0x3DCCCCCD;
        }
        break;
    case 0x16:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(char *)(arg0 + 4) = 1;
            *(char *)(arg0 + 5) = 0x1B;
            *(unsigned char *)(arg0 + 6) = 0xA;
            *(char *)(arg0 + 7) = 0;
            *(char *)(arg0 + 0x1F0) = 0x2F;
            *(short *)(arg0 + 0x20E) = 0x3C;
            func_001749A0(arg0, func_00188610(arg0), 0, 16.0f);
        } else {
            *(int *)(arg0 + 0x204) = 0x3E800000;
        }
        break;
    }
}
