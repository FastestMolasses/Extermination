// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: per-area camera driver for entity sub-states 29..37 (called from
// func_00195130, the outer camera dispatcher; sibling of func_00194D10).
//   act = camera/controller block, ent = tracked entity, idx = area/preset index.
//   act+0x10..0x18 = camera position (x,y,z), act+0x44 = yaw, act+0xC = distance,
//   act+0x94 = distance bias, act+0x64 = a height marker, act+0x20 = look-at target.
//   ent+0x230 = the entity's behaviour sub-state (the switch selector),
//   ent+0xA0/0xA8 = entity XZ, ent+0xC4 = entity facing.
//   D_0024A530 is a stride-20 (5-float) per-area preset table:
//     +0x00/+0x04/+0x08 = default camera x/y/z, +0x0C = base yaw, +0x10 = ref facing.
//   D_008105D0 = the shared "camera look-at" vector; D_008105E0/E8 = orbit centre XZ;
//   D_00810690 = the scripted-approach distance; D_00810700/0701 = area / sub-area id.
//   0x70003A20 / 0x70003A24 and 0x700038A0.. are the EE scratchpad staging vectors.
// Returns 1 when the state was handled, 0 otherwise.
// Cases 36/37 fall through into the 30/31/34/35 arm when the area gate fails.

extern float func_001B1240(unsigned char *, float, float);
extern void  func_0018C6A0(unsigned char *, float *, float);
extern void  func_0018C4B0(float *, float, float);
extern void  func_0018D7B0(unsigned char *, int);
extern void  func_00102948(float *, unsigned char *);
extern float func_0011DF78(float);
extern void  func_001028D0(float *, unsigned char *, unsigned char *);
extern void  func_00102760(float *, float *);
extern float func_001B1470(float);
extern float func_001B12B0(float, float, float);
extern float func_00191120(int, float, float, float, float);
extern float func_0011E2A8(float);
extern float func_0011DE90(float);

extern float D_0024A530[];
extern float D_0024A534[];
extern float D_0024A538[];
extern float D_0024A53C[];
extern float D_0024A540[];
extern float D_008105D0[];
extern float D_008105E0;
extern float D_008105E8;
extern float D_00810690;
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern float D_700038A0[];

int func_001944B0(unsigned char *act, unsigned char *ent, int idx)
{
    int state;
    int off;
    int mode;
    float t;
    float lim;
    float a;
    float c;
    float sum;

    state = *(int *)(ent + 0x230);
    switch (state) {
    case 29:
        if (idx >= 7) {
            *(float *)(act + 0x14) = D_0024A534[idx * 5];
            *(float *)(act + 0x44) = func_001B1240(act + 0x10, *(float *)(ent + 0xA0), *(float *)(ent + 0xA8));
            func_0018C6A0(act + 0x10, D_008105D0, 0.800000012f);
            func_0018C4B0(D_008105D0, *(float *)(act + 0x14), 0.800000012f);
            func_0018D7B0(act, 5);
        } else {
            off = idx * 5;
            *(float *)(act + 0x10) = D_0024A530[off];
            *(float *)(act + 0x14) = D_0024A534[off];
            *(float *)(act + 0x18) = D_0024A538[off];
            *(float *)(act + 0x44) = func_001B1240(act + 0x10, *(float *)(ent + 0xA0), *(float *)(ent + 0xA8));
            func_0018D7B0(act, 5);
            func_00102948(D_008105D0, act + 0x10);
        }
        return 1;

    case 32:
    case 33:
        t = D_00810690 - func_0011DF78(*(float *)(act + 0xC));
        *(volatile float *)0x70003A20 = t;
        if (t > 0.0f) {
            func_001028D0(D_700038A0, act + 0x20, act + 0x10);
            *(volatile int *)0x700038A4 = 0;
            *(volatile int *)0x700038AC = 0;
            func_00102760(D_700038A0, D_700038A0);
            *(float *)(act + 0x10) = *(float *)(act + 0x10) + *(volatile float *)0x700038A0 * *(volatile float *)0x70003A20;
            *(float *)(act + 0x18) = *(float *)(act + 0x18) + *(volatile float *)0x700038A8 * *(volatile float *)0x70003A20;
        } else {
            if (*(float *)(act + 0x64) == -46.7999992f) {
                lim = -30.0f;
            } else {
                lim = -20.0f;
            }
            if (t < lim) {
                *(volatile float *)0x70003A24 = t - lim;
                func_001028D0(D_700038A0, act + 0x20, act + 0x10);
                *(volatile int *)0x700038A4 = 0;
                *(volatile int *)0x700038AC = 0;
                func_00102760(D_700038A0, D_700038A0);
                *(float *)(act + 0x10) = *(float *)(act + 0x10) + *(volatile float *)0x700038A0 * *(volatile float *)0x70003A24;
                *(float *)(act + 0x18) = *(float *)(act + 0x18) + *(volatile float *)0x700038A8 * *(volatile float *)0x70003A24;
            }
        }
        func_0018D7B0(act, 4);
        *(float *)(act + 0x14) = D_0024A534[idx * 5];
        func_0018C4B0(D_008105D0, *(float *)(act + 0x14), 0.800000012f);
        func_0018C6A0(act + 0x10, D_008105D0, 0.800000012f);
        return 1;

    case 36:
    case 37:
        mode = D_00810700;
        if (mode == 8 || (mode == 0x13 && D_00810701 == 1)) {
            t = D_00810690 - func_0011DF78(*(float *)(act + 0xC));
            *(volatile float *)0x70003A20 = t;
            if (t > 0.0f) {
                func_001028D0(D_700038A0, act + 0x20, act + 0x10);
                *(volatile int *)0x700038A4 = 0;
                *(volatile int *)0x700038AC = 0;
                func_00102760(D_700038A0, D_700038A0);
                *(float *)(act + 0x10) = *(float *)(act + 0x10) + *(volatile float *)0x700038A0 * *(volatile float *)0x70003A20;
                *(float *)(act + 0x18) = *(float *)(act + 0x18) + *(volatile float *)0x700038A8 * *(volatile float *)0x70003A20;
            } else {
                if (*(float *)(act + 0x64) == -46.7999992f) {
                    lim = -30.0f;
                } else {
                    lim = -20.0f;
                }
                if (t < lim) {
                    *(volatile float *)0x70003A24 = t - lim;
                    func_001028D0(D_700038A0, act + 0x20, act + 0x10);
                    *(volatile int *)0x700038A4 = 0;
                    *(volatile int *)0x700038AC = 0;
                    func_00102760(D_700038A0, D_700038A0);
                    *(float *)(act + 0x10) = *(float *)(act + 0x10) + *(volatile float *)0x700038A0 * *(volatile float *)0x70003A24;
                    *(float *)(act + 0x18) = *(float *)(act + 0x18) + *(volatile float *)0x700038A8 * *(volatile float *)0x70003A24;
                }
            }
            func_0018D7B0(act, 4);
            *(float *)(act + 0x14) = D_0024A534[idx * 5];
            func_0018C4B0(D_008105D0, *(float *)(act + 0x14), 0.800000012f);
            func_0018C6A0(act + 0x10, D_008105D0, 0.800000012f);
            return 1;
        }
        /* fallthrough */
    case 30:
    case 31:
    case 34:
    case 35:
        if (state == 30) {
            if (func_0011DF78(func_001B1470(*(float *)(ent + 0xC4) - D_0024A540[idx * 5])) <= 1.57079637f) {
                *(float *)(act + 0x44) = func_001B12B0(
                    func_001B1470(D_0024A53C[idx * 5] - 0.52359879f),
                    *(float *)(act + 0x44), 0.00523598818f);
            } else {
                *(float *)(act + 0x44) = func_001B12B0(
                    func_001B1470(0.52359879f + D_0024A53C[idx * 5]),
                    *(float *)(act + 0x44), 0.00523598818f);
            }
            c = func_0011E2A8(*(float *)(act + 0x44));
            sum = *(float *)(act + 0xC) + *(float *)(act + 0x94);
            *(float *)(act + 0x10) = D_008105E0 + sum * c;
            c = func_0011DE90(*(float *)(act + 0x44));
            sum = *(float *)(act + 0xC) + *(float *)(act + 0x94);
            *(float *)(act + 0x18) = D_008105E8 + sum * c;
        } else {
            a = func_00191120(state, D_0024A53C[idx * 5], *(float *)(act + 0x44), 0.0349065848f, 0.52359879f);
            if (a != *(float *)(act + 0x44)) {
                *(float *)(act + 0x44) = a;
                c = func_0011E2A8(a);
                sum = *(float *)(act + 0xC) + *(float *)(act + 0x94);
                *(float *)(act + 0x10) = D_008105E0 + sum * c;
                c = func_0011DE90(*(float *)(act + 0x44));
                sum = *(float *)(act + 0xC) + *(float *)(act + 0x94);
                *(float *)(act + 0x18) = D_008105E8 + sum * c;
            }
        }
        if (idx == 0) {
            if (*(float *)(act + 0x10) < -365.0f) {
                *(float *)(act + 0x10) = -365.0f;
            }
        } else if (idx == 3) {
            if (*(float *)(act + 0x10) < 749.5f) {
                *(float *)(act + 0x10) = 749.5f;
            }
        } else if (idx == 6) {
            if (*(float *)(act + 0x18) < 1010.0f) {
                *(float *)(act + 0x18) = 1010.0f;
            }
        }
        *(float *)(act + 0x14) = D_0024A534[idx * 5];
        func_0018C4B0(D_008105D0, *(float *)(act + 0x14), 0.800000012f);
        func_0018C6A0(act + 0x10, D_008105D0, 0.800000012f);
        func_0018D7B0(act, 5);
        return 1;
    }
    return 0;
}
