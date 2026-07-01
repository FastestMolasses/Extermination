// NEARMISS func_0016CD70  (vram 0x0016CD70, 0x3B4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.19% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 99.19% on mwcc233, logic and structure fully recovered (matrix/quat blend setup via func_001029C0/func_00102BB0/func_001026A0, angle-delta gate at 0.5235988f, position integration, yaw-facing pitch correction with -0.6f trim, footstep/state-machine dispatch on func_00175900 return, the func_0017B...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001026A0(float *a, float *b, float *c);
extern void func_001029C0(float *a);
extern void func_00102BB0(float *a, float *b, float c);
extern float func_0011DF78(float x);
extern float func_0011E2A8(float x);
extern void func_0016C520(unsigned char *e);
extern int func_00175900(unsigned char *e, int x);
extern void func_001791D0(unsigned char *e, int x);
extern float func_001B1470(float x);
extern void func_001EFD90(int id, unsigned char *a, unsigned char *b);
extern int func_001FBD50(unsigned char *e, int idx, int a, float b);
extern float D_700036A0[];
extern float D_700038A0[];
extern float D_700038B0[];

int func_0016CD70(unsigned char *e, int arg1) {
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0;
    *(float *)0x700038A8 = *(float *)(e + 0x38);
    *(int *)0x700038AC = 0;
    func_001029C0(D_700036A0);
    if (func_0011DF78(func_001B1470(*(float *)(e + 0x218) - *(float *)(e + 0xC4))) <= 0.5235988f) {
        func_00102BB0(D_700036A0, D_700036A0, *(float *)(e + 0xC4));
    } else {
        func_00102BB0(D_700036A0, D_700036A0, *(float *)(e + 0x218));
    }
    func_001026A0(D_700038B0, D_700036A0, D_700038A0);
    *(float *)(e + 0xB0) = *(float *)(e + 0xB0) + *(float *)0x700038B0;
    *(float *)(e + 0xB8) = *(float *)(e + 0xB8) + *(float *)0x700038B8;
    func_001791D0(e, 0);
    *(float *)(e + 0xB4) = *(float *)(e + 0xB4) - (*(float *)(e + 0x38) * func_0011E2A8(*(float *)(e + 0x9C)));
    *(float *)(e + 0xB4) += -0.6f;
    *(float *)(e + 0xB4) = *(float *)(e + 0xB4) + *(float *)(e + 0x2EC);
    if (func_00175900(e, 0) != 0) {
        if (*(unsigned char *)(e + 0x237) == 0) {
            float zero = 0.0f;
            if (*(float *)(e + 0x38) != zero) {
                float r = *(float *)(e + 0x38) / 0.75f;
                *(float *)(e + 0x26C) = r;
                if (!(r <= 1.0f)) {
                    *(float *)(e + 0x26C) = 1.0f;
                }
                *(float *)(e + 0x38) = 0.0f;
            }
            if (arg1 == 0 && (int)*(unsigned char *)(e + 6) >= 3) {
                *(unsigned char *)(e + 6) = 0xA;
                *(unsigned char *)(e + 0x1F0) = 0;
                func_0016C520(e);
                return 1;
            }
        } else {
            *(float *)(e + 0x2EC) = 0.0f;
        }
    } else {
        float v = *(float *)(e + 0x2EC);
        if (!(v <= -0.19999999f)) {
            *(float *)(e + 0x2EC) = v + -0.04f;
        } else if (arg1 == 0 && (int)*(unsigned char *)(e + 6) >= 3) {
            *(unsigned char *)(e + 6) = 0x14;
            *(unsigned char *)(e + 0x1F0) = 0xB;
            func_0016C520(e);
            return 2;
        }
    }

    if (*(unsigned char *)(e + 0x31A) != 0 && *(unsigned short *)(e + 0x31C) == 0x12E && *(signed char *)(e + 0x31B) == -1) {
        *(unsigned char *)(e + 0x31B) = func_001FBD50(e, 0x12E, 0, 300.0f);
    }
    *(short *)(e + 0x2A) = *(short *)(e + 0x2A) + 1;
    if (!(*(short *)(e + 0x2A) & 7)) {
        int t = *(unsigned char *)(e + 0x23A);
        switch (t) {
        case 0x0:
        case 0x1:
        case 0x2:
        case 0x3:
        case 0x4:
        case 0xD:
        case 0xE:
        default:
            func_001EFD90(0x80000012, e + 0xB0, e + 0xC0);
            break;
        case 0x5:
        case 0x5A:
            func_001EFD90(0x80000065, e + 0xB0, e + 0xC0);
            break;
        case 0x8:
        case 0x5C:
            func_001EFD90(0x80000066, e + 0xB0, e + 0xC0);
            break;
        case 0x6:
        case 0x5B:
        case 0x7:
            func_001EFD90(0x80000033, e + 0xB0, e + 0xC0);
            break;
        }
    }
    return 0;
}
