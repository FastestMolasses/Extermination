// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001749A0(unsigned char *e, int clip, int mode, float speed);
extern int func_00224290(unsigned char *e);
extern int func_0017C860(unsigned char *e, float y);
extern void func_001764E0(unsigned char *e);
extern void func_00179880(unsigned char *e, void *p);
extern int func_00175900(unsigned char *e, int f);
extern void func_0017C580(unsigned char *e);
extern void func_0021D250(unsigned char *e, int b);
extern void func_0021D2E0(unsigned char *e, int a, int b);

void func_001639E0(unsigned char *e) {
    int landed;
    int near_top = 0;

    switch (e[6]) {
    case 0:
        e[6]++;
        e[7] = 0;
        *(int *)(e + 0x38) = 0;
        e[0x25C] = 0;
        func_001749A0(e, 0x72, 0, 8.0f);
    case 1:
        landed = func_00224290(e);
        if (e[0x23B] == 0x39 && !(*(float *)(e + 0xB4) < *(float *)(e + 0x2F4) - 12.0f)) {
            near_top = 1;
        }
        if (landed || near_top || func_0017C860(e, *(float *)(e + 0x2EC)) == 0) {
            func_001764E0(e);
            func_00179880(e, e + 0x2EC);
            func_00175900(e, 1);
            if (e[0xA] && !landed) {
                func_0017C580(e);
            }
            if (e[0x23A] == 0x5D) {
                func_0021D250(e, 0);
            }
        }
        break;
    case 2:
        func_0021D2E0(e, 0x78, 0);
        break;
    }
}
