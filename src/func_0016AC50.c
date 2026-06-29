// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Entity state-machine tick (state in arg0[6]): case0 inits fall timer + per-frame delta, case1 fires sound when flag set, case2 advances/decrements fall over 0x28 frames, case3 transitions on flag.
extern void func_001749A0(unsigned char *, int, int, float);
extern int func_001885B0(unsigned char *);
extern void func_001FBD50(unsigned char *, int, int, float);

void func_0016AC50(unsigned char *arg0) {
    unsigned char state;

    state = arg0[6];
    switch (state) {
    case 0:
        arg0[6] = (unsigned char)(state + 1);
        arg0[7] = 0;
        *(short *)(arg0 + 0x28) = 8;
        *(float *)(arg0 + 0x2E4) = (*(float *)(arg0 + 0x254) - *(float *)(arg0 + 0xB4)) / 8.0f;
        func_001749A0(arg0, 0x70, 0, 1.0f);
        break;
    case 1:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[6] = (unsigned char)(state + 1);
            func_001749A0(arg0, 0xBA, 0, 1.0f);
            arg0[0x25F] = 3;
        }
        break;
    case 2:
        if (*(short *)(arg0 + 0x28) == 0) {
            arg0[6] = (unsigned char)(state + 1);
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0x254);
            func_001FBD50(arg0, 0x110, 0, 300.0f);
        } else {
            float d = *(float *)(arg0 + 0x2E4);
            d = d;
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + d;
            *(short *)(arg0 + 0x28) = (short)(*(short *)(arg0 + 0x28) - 1);
        }
        break;
    case 3:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[5] = 0x12;
            arg0[6] = 0;
            arg0[0x1F0] = 0x22;
            arg0[0x1F1] = 0;
            arg0[0x25D] = 0;
            arg0[0x2F1] = 0;
            func_001749A0(arg0, func_001885B0(arg0), 0, 1.0f);
        }
        break;
    }
}