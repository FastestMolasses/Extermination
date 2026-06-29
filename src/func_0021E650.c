// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// State-machine dispatcher on *(u8*)(p+7): for each state 0..4, if the float
// at p+0x3C is <= a per-state threshold, advance the state byte and fire the
// state's effect (func_001B61C0 burst + per-state follow-up). Default: nothing.
extern void func_00182870(int, int);
extern void func_001B61C0(int, int, int, int);
extern void func_001FBD50(int, int, float, int);
extern void func_0021D490(int);

void func_0021E650(unsigned char *p) {
    int st = *(unsigned char *)(p + 7);
    switch (st) {
    case 0:
        if (*(float *)(p + 0x3C) <= 173.0f) {
            *(unsigned char *)(p + 7) = st + 1;
            func_001B61C0(0, 0xC0, 5, 1);
            func_00182870((int)p, 1);
        }
        break;
    case 1:
        if (*(float *)(p + 0x3C) <= 105.0f) {
            *(unsigned char *)(p + 7) = st + 1;
            func_001B61C0(0, 0xC0, 5, 1);
        }
        break;
    case 2:
        if (*(float *)(p + 0x3C) <= 85.0f) {
            *(unsigned char *)(p + 7) = st + 1;
            func_001B61C0(0, 0xC0, 5, 1);
        }
        break;
    case 3:
        if (*(float *)(p + 0x3C) <= 60.0f) {
            *(unsigned char *)(p + 7) = st + 1;
            func_001B61C0(0, 0xC0, 5, 1);
            func_001FBD50((int)p, 0x156, 300.0f, 0);
        }
        break;
    case 4:
        if (*(float *)(p + 0x3C) <= 45.0f) {
            *(unsigned char *)(p + 7) = st + 1;
            func_001B61C0(0, 0xD0, 0xA, 1);
            func_0021D490((int)p);
        }
        break;
    }
}
