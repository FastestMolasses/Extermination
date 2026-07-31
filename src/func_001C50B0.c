// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Flicker-light actor tick: state 0 picks the light colour/flicker amplitude from the
// current area id and registers the light, state 1 jitters the colour each frame, states
// 2/3 release the light and destroy the actor.

extern int func_001028B8(char *a, char *b, void *c);
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
            break;
        }
        *(unsigned char *)(arg0 + 4) = func_001F5490();
        if (*(unsigned char *)(arg0 + 4) == 3) {
            break;
        }
        f20 = 0.0f;
        mode = (D_00810700 << 8) + D_00810701;
        switch (mode) {
        case 0xF00:
            switch (*(unsigned char *)(arg0 + 0xD)) {
            case 1:
            default:
                *(int *)(arg0 + 0x80) = 0x3F800000;
                *(int *)(arg0 + 0x84) = 0x3F800000;
                *(int *)(arg0 + 0x88) = 0x3F800000;
                *(int *)(arg0 + 0x8C) = 0x3E800000;
                break;
            case 0:
                *(int *)(arg0 + 0x80) = 0x3F333333;
                *(int *)(arg0 + 0x84) = 0;
                *(int *)(arg0 + 0x88) = 0;
                *(int *)(arg0 + 0x8C) = 0x3E800000;
                f20 = 1.25f;
                break;
            }
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
        if (f20 != 0.0f) {
            switch (mode) {
            case 0x601:
            case 0x600:
                if (*(unsigned char *)(arg0 + 0xD) == 0) {
                    {
                        char *p;
                        void *g;
                        p = arg0 + 0xB0;
                        g = &D_0026E2E0;
                        func_001028B8(p, p, g);
                    }
                    *(int *)(arg0 + 0x20) = func_001C5050(arg0, f20);
                    {
                        char *p2;
                        void *g2;
                        p2 = arg0 + 0xB0;
                        g2 = &D_0026E2E0;
                        func_001028D0(p2, p2, g2);
                    }
                } else {
                    *(int *)(arg0 + 0x20) = func_001C5050(arg0, f20);
                }
                break;
            default:
                *(int *)(arg0 + 0x20) = func_001C5050(arg0, f20);
                break;
            }
        } else {
            *(int *)(arg0 + 0x20) = -1;
        }
        func_00102900(arg0 + 0x80, arg0 + 0x80, 128.0f);
        break;
    case 1:
        if (func_001C4FA0() != 0) {
            *(unsigned char *)(arg0 + 4) = 3;
            break;
        }
        f20 = *(float *)(arg0 + 0x8C);
        f21 = f20;
        f20 = -f20;
        f2 = f20 + (f21 - f20) * (4.656613e-10f * (float)func_00122BB8());

        v = *(float *)(arg0 + 0x80) + f2;
        v = v < 0.0f ? 0.0f : v;
        v = v > 255.0f ? 255.0f : v;
        sp30.x = v;

        v = *(float *)(arg0 + 0x84) + f2;
        v = v < 0.0f ? 0.0f : v;
        v = v > 255.0f ? 255.0f : v;
        sp30.y = v;

        v = *(float *)(arg0 + 0x88) + f2;
        v = v < 0.0f ? 0.0f : v;
        v = v > 255.0f ? 255.0f : v;
        sp30.z = v;

        sp30.w = 0;
        func_001F5F60(arg0 + 0xB0, arg0 + 0xC0, &sp30.x, *(int *)(arg0 + 0x44));
        break;
    case 2:
    case 3:
    default:
        idx = *(int *)(arg0 + 0x20);
        if (idx != -1) {
            func_001D80B0(idx);
        }
        func_001AFC10(arg0);
        break;
    }
}
