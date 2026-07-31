// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Entity state-machine tick: advances the 4-state counter at p[7] (spawn cue, timer
// fade-out at p+0x38, ready/idle branch on p[0x23F], teardown), then runs the common
// per-frame tail (update, p+0xB4 -= 0.2f, animate, post).

extern void func_001749A0(char *p, int code, int flags, float blend);
extern void func_00174AC0(char *p, int a);
extern void func_00175900(char *p, int a);
extern void func_001764E0(char *p);
extern void func_00178B90(char *p, int a);
extern void func_001796C0(char *p);
extern void func_0017C440(char *p, int a);
extern void func_0017C540(char *p);
extern float D_0024889C;

void func_00164220(char *p) {
    unsigned char st;
    float t;

    st = *(unsigned char *)(p + 7);
    switch (st) {
    case 0:
        *(unsigned char *)(p + 7) = st + 1;
        func_001749A0(p, 0x74, 0, 1.0f);
        *(float *)(p + 0x38) = 0.8f;
        break;
    case 1:
        if (*(int *)(p + 0x200) & 0x1000) {
            *(unsigned char *)(p + 7) = st + 1;
        } else {
            t = *(float *)(p + 0x38) - D_0024889C;
            *(float *)(p + 0x38) = t;
            if (t <= 0.0f) {
                *(float *)(p + 0x38) = 0.0f;
            }
            func_00178B90(p, 0);
        }
        break;
    case 2:
        func_00174AC0(p, 0);
        if ((int)*(unsigned char *)(p + 0x23F) > 1) {
            *(unsigned char *)(p + 7) = *(unsigned char *)(p + 7) + 1;
            func_0017C440(p, 0);
        } else {
            *(char *)(p + 0x25C) = 0;
            func_0017C540(p);
        }
        break;
    case 3:
        func_00178B90(p, 0);
        if (!(*(int *)(p + 0x200) & 0x8000)) {
            func_0017C540(p);
        }
        break;
    }
    func_001764E0(p);
    *(float *)(p + 0xB4) += -0.2f;
    func_00175900(p, 1);
    func_001796C0(p);
}
