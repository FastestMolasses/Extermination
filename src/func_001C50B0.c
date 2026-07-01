// NEARMISS func_001C50B0  (vram 0x001C50B0, 0x4BC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.24% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Three residual clusters at 88.24% (both compilers agree on cluster 1, ruling out a build-version fix): (1) outer state-dispatch paddub-into-delay-slot placement; (2) f20==0.0 guard should be bc1tl/branch-likely but mwcc233 emits plain bc1f even after early-return reshaping; (3) case1's f20/f21/f2...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_001028B8(char *a, char *b, void *c, float f);
extern int func_001028D0(char *a, char *b, void *c);
extern int func_00102900(char *a, char *b, float f);
extern int func_00122BB8(void);
extern void func_001AFC10(char *arg0);
extern int func_001C4FA0(void);
extern int func_001C5050(char *arg0, float f);
extern void func_001D80B0(int a);
extern unsigned char func_001F5490(void);
extern int func_001F5F60(char *a, char *b, float *c, int d);

extern float D_0026E2E0;
extern unsigned char D_00810700;
extern unsigned char D_00810701;

void func_001C50B0(char *arg0) {
    float f20;
    float f21;
    float f2;
    float v;
    struct { float x, y, z; int w; } sp30;
    int idx;
    int mode;

    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        if (func_001C4FA0() != 0) {
            *(unsigned char *)(arg0 + 4) = 3;
            return;
        }
        *(unsigned char *)(arg0 + 4) = func_001F5490();
        if (*(unsigned char *)(arg0 + 4) == 3) {
            return;
        }
        f20 = 0.0f;
        mode = (D_00810700 << 8) + D_00810701;
        switch (mode) {
        case 0xF00:
            if (*(unsigned char *)(arg0 + 0xD) != 0) {
                *(int *)(arg0 + 0x80) = 0x3F800000;
                *(int *)(arg0 + 0x84) = 0x3F800000;
                *(int *)(arg0 + 0x88) = 0x3F800000;
                *(int *)(arg0 + 0x8C) = 0x3E800000;
                break;
            }
            *(int *)(arg0 + 0x80) = 0x3F333333;
            *(int *)(arg0 + 0x84) = 0;
            *(int *)(arg0 + 0x88) = 0;
            *(int *)(arg0 + 0x8C) = 0x3E800000;
            f20 = 1.25f;
            break;
        case 0x601:
        case 0x600:
            if (*(unsigned char *)(arg0 + 0xD) != 0) {
                *(int *)(arg0 + 0x80) = 0;
                *(int *)(arg0 + 0x84) = 0x3F800000;
                *(int *)(arg0 + 0x88) = 0;
                *(int *)(arg0 + 0x8C) = 0x3E800000;
                break;
            }
            *(int *)(arg0 + 0x80) = 0;
            *(int *)(arg0 + 0x84) = 0x3F000000;
            *(int *)(arg0 + 0x88) = 0;
            *(int *)(arg0 + 0x8C) = 0x3E800000;
            f20 = 9.2f;
            break;
        case 0x100:
            if (*(unsigned char *)(arg0 + 0xD) != 0) {
                *(int *)(arg0 + 0x80) = 0;
                *(int *)(arg0 + 0x84) = 0x3F800000;
                *(int *)(arg0 + 0x88) = 0;
                *(int *)(arg0 + 0x8C) = 0x3E800000;
                break;
            }
            *(int *)(arg0 + 0x80) = 0x3F800000;
            *(int *)(arg0 + 0x84) = 0x3F800000;
            *(int *)(arg0 + 0x88) = 0x3F800000;
            *(int *)(arg0 + 0x8C) = 0x3E800000;
            f20 = 1.2f;
            break;
        case 0x703:
            if (*(unsigned char *)(arg0 + 0xD) != 0) {
                *(int *)(arg0 + 0x80) = 0;
                *(int *)(arg0 + 0x84) = 0x3F800000;
                *(int *)(arg0 + 0x88) = 0;
                *(int *)(arg0 + 0x8C) = 0x3E800000;
                break;
            }
            *(int *)(arg0 + 0x80) = 0;
            *(int *)(arg0 + 0x84) = 0x3F000000;
            *(int *)(arg0 + 0x88) = 0;
            *(int *)(arg0 + 0x8C) = 0x3E800000;
            f20 = 1.2f;
            break;
        case 0x803:
            if (*(unsigned char *)(arg0 + 0xD) != 1) {
                *(int *)(arg0 + 0x80) = 0;
                *(int *)(arg0 + 0x84) = 0x3F800000;
                *(int *)(arg0 + 0x88) = 0;
                *(int *)(arg0 + 0x8C) = 0x3E800000;
                break;
            }
            *(int *)(arg0 + 0x80) = 0;
            *(int *)(arg0 + 0x84) = 0x3F000000;
            *(int *)(arg0 + 0x88) = 0;
            *(int *)(arg0 + 0x8C) = 0x3E800000;
            f20 = 1.2f;
            break;
        default:
            *(int *)(arg0 + 0x80) = 0;
            *(int *)(arg0 + 0x84) = 0x3F800000;
            *(int *)(arg0 + 0x88) = 0;
            *(int *)(arg0 + 0x8C) = 0x3E800000;
            break;
        }
        if (f20 == 0.0f) {
            idx = -1;
        } else if (mode != 0x600 && mode != 0x601) {
            idx = func_001C5050(arg0, f20);
        } else if (*(unsigned char *)(arg0 + 0xD) == 0) {
            func_001028B8(arg0 + 0xB0, arg0 + 0xB0, &D_0026E2E0, f20);
            idx = func_001C5050(arg0, f20);
            *(int *)(arg0 + 0x20) = idx;
            func_001028D0(arg0 + 0xB0, arg0 + 0xB0, &D_0026E2E0);
            func_00102900(arg0 + 0x80, arg0 + 0x80, 128.0f);
            return;
        } else {
            idx = func_001C5050(arg0, f20);
        }
        *(int *)(arg0 + 0x20) = idx;
        func_00102900(arg0 + 0x80, arg0 + 0x80, 128.0f);
        return;
    case 1:
        if (func_001C4FA0() != 0) {
            *(unsigned char *)(arg0 + 4) = 3;
            return;
        }
        f21 = *(float *)(arg0 + 0x8C);
        f20 = -f21;
        f2 = f20 + (f21 - f20) * (4.656613e-10f * (float)func_00122BB8());

        v = *(float *)(arg0 + 0x80) + f2;
        if (v < 0.0f) {
            v = 0.0f;
        }
        if (!(v <= 255.0f)) {
            v = 255.0f;
        }
        sp30.x = v;

        v = *(float *)(arg0 + 0x84) + f2;
        if (v < 0.0f) {
            v = 0.0f;
        }
        if (!(v <= 255.0f)) {
            v = 255.0f;
        }
        sp30.y = v;

        v = *(float *)(arg0 + 0x88) + f2;
        if (v < 0.0f) {
            v = 0.0f;
        }
        if (!(v <= 255.0f)) {
            v = 255.0f;
        }
        sp30.z = v;

        sp30.w = 0;
        func_001F5F60(arg0 + 0xB0, arg0 + 0xC0, (float *)&sp30, *(int *)(arg0 + 0x44));
        return;
    case 2:
    case 3:
        idx = *(int *)(arg0 + 0x20);
        if (idx != -1) {
            func_001D80B0(idx);
        }
        func_001AFC10(arg0);
        break;
    }
}
