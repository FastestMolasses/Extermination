// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Per-entity state machine (dispatch on *(arg0+6)): spawn/wait/advance via anim-clip
// threshold tables D_00248700/D_00248704, then rewind (func_0017C440) or reset
// (func_0017C540); always runs func_001764E0, decays arg0+0xB4 by 0.2, func_00175900,
// func_001796C0.

extern int func_001749A0(unsigned char *e, short clip, int flags, float blend);
extern void func_00173DD0(unsigned char *self);
extern unsigned char func_00174AC0(unsigned char *arg0, int arg1);
extern void func_0017C440(unsigned char *p, int unused);
extern void func_0017C540(unsigned char *a0);
extern void func_0011A070(int arg0);
extern int func_001FBD50(void *a0, int a1, int a2, float f12);
extern void func_00175900(unsigned char *a0, int a1);
extern void func_001764E0(unsigned char *a0);
extern void func_00178B90(unsigned char *a0, int a1);
extern void func_001796C0(unsigned char *a0);
extern float D_00248700[256][2];
extern float D_00248704[256][2];
extern short D_002754A8[2];

void func_00173E60(unsigned char *arg0) {
    unsigned char st;

    st = arg0[6];
    switch (st) {
    case 0:
        arg0[6] = st + 1;
        arg0[7] = 0;
        *(int *)(arg0 + 0x38) = 0;
        func_001749A0(arg0, D_002754A8[*(unsigned char *)(arg0 + 0x236)], 0, *(float *)(arg0 + 0x1FC));
        *(unsigned char *)(*(char **)(arg0 + 0x18) + 0xA) = 0;
        *(unsigned char *)(arg0 + 0x302) = 0xFF;
        break;
    case 1:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            arg0[6] = st + 1;
        }
        break;
    case 2:
        func_00173DD0(arg0);
        if (*(float *)(arg0 + 0x3C) <= D_00248700[*(unsigned char *)(arg0 + 0x236)][0]) {
            arg0[6] = arg0[6] + 1;
            *(char *)(*(char **)(arg0 + 0x18) + 0x0) = 1;
            *(short *)(*(char **)(arg0 + 0x18) + 0x36) = 0xF;
            *(unsigned char *)(arg0 + 0x302) = func_001FBD50(arg0, 0x17F, 0, 300.0f);
            *(unsigned char *)(arg0 + 0x25E) = 0x83;
        }
        break;
    case 3:
        if (*(unsigned char *)(*(char **)(arg0 + 0x18) + 0xA) != 0) {
            arg0[6] = 0x50;
            {
                unsigned char idx = *(unsigned char *)(arg0 + 0x302);
                if ((int)idx != -1) {
                    func_0011A070(idx);
                    *(unsigned char *)(arg0 + 0x302) = 0xFF;
                }
            }
        } else {
            func_00173DD0(arg0);
            if (*(float *)(arg0 + 0x3C) <= D_00248704[*(unsigned char *)(arg0 + 0x236)][0]) {
                arg0[6] = arg0[6] + 1;
                *(char *)(*(char **)(arg0 + 0x18) + 0x0) = 2;
            }
        }
        break;
    case 4:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[6] = 0x63;
        }
        break;
    case 0x50:
        arg0[6] = st + 1;
        *(short *)(arg0 + 0x28) = 4;
        /* fallthrough */
    case 0x51:
        {
            short v = *(short *)(arg0 + 0x28);
            *(short *)(arg0 + 0x28) = v - 1;
            if (v == 0) {
                arg0[6] = arg0[6] + 1;
                if (*(unsigned char *)(arg0 + 0x236) == 0) {
                    func_001749A0(arg0, 0x10F, 0.0f, 4.0f);
                } else {
                    func_001749A0(arg0, 0x1C1, 0.0f, 4.0f);
                }
            } else {
                *(int *)(arg0 + 0x204) = 0;
            }
        }
        break;
    case 0x52:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[6] = 0x63;
        }
        break;
    case 0x63:
        func_00174AC0(arg0, 1);
        if (*(unsigned char *)(arg0 + 0x23F) > 1) {
            arg0[6] = arg0[6] + 1;
            func_0017C440(arg0, 0);
        } else {
            *(char *)(arg0 + 0x25C) = 0;
            func_0017C540(arg0);
        }
        break;
    case 0x64:
        func_00174AC0(arg0, 1);
        func_00178B90(arg0, 0);
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            func_0017C540(arg0);
        }
        break;
    }
    func_001764E0(arg0);
    *(float *)(arg0 + 0xB4) += -0.2f;
    func_00175900(arg0, 1);
    func_001796C0(arg0);
}
