// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
extern void func_001749A0(unsigned char *e, short clip, int flags, float blend);
extern int func_00175900(unsigned char *e, int flag);
extern void func_00178B90(unsigned char *e, int f);
extern void func_00179880(unsigned char *e, void *p);
extern void func_0017C540(unsigned char *e);
extern void func_00182430(unsigned char *e, int g);
extern void func_001B61C0(int a, int b, int c, int d);
extern void func_001FBD50(unsigned char *e, int b, int c, float f);
extern void func_0021D250(unsigned char *e, int b);

extern float **D_00275B40;
extern unsigned char D_008106F1[16];

void func_00220B50(unsigned char *e) {
    unsigned char state = *(unsigned char *)(e + 6);

    switch (state) {
    case 0:
        func_001FBD50(e, 0x150, 0, 300.0f);
        *(unsigned char *)(e + 6) = *(unsigned char *)(e + 6) + 1;
        *(unsigned char *)(e + 7) = 0;
        func_001749A0(e, 0x37, 0, 1.0f);
        *(float *)(e + 0x38) = 0.0f;
        *(float *)(e + 0x21C) = 0.0f;
        *(int *)(e + 0x2EC) = 0;
        break;
    case 1:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(unsigned char *)(e + 6) = state + 1;
            func_00182430(e, 2);
            func_001B61C0(0, 0xC0, 5, 1);
        } else {
            *(float *)(e + 0x38) = (*D_00275B40)[2] - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = (*D_00275B40)[2];
            func_00178B90(e, 1);
            func_00179880(e, e + 0x2EC);
            func_00175900(e, 1);
        }
        break;
    case 2:
        func_00179880(e, e + 0x2EC);
        if (func_00175900(e, 1) != 0) {
            if (!(*(float *)(e + 0x228) < 100.0f) && D_008106F1[0] != 0) {
                *(unsigned char *)(e + 4) = 2;
                *(unsigned char *)(e + 5) = 2;
                *(unsigned char *)(e + 6) = 0;
                *(unsigned char *)(e + 0x1F0) = 0x3E;
                *(unsigned char *)(e + 0xF) = 0;
                return;
            }
            *(unsigned char *)(e + 6) = *(unsigned char *)(e + 6) + 1;
        }
        break;
    case 3:
        *(int *)(e + 0x224) = 0;
        *(int *)(e + 0x22C) = 0;
        *(unsigned char *)(e + 0xF) = 0;
        *(short *)(e + 0x20E) = 0x3C;
        *(unsigned char *)(e + 0x25C) = 0;
        func_0017C540(e);
        break;
    }

    if (*(unsigned char *)(e + 0x23A) == 0x5D) {
        func_0021D250(e, 0);
    }
}
