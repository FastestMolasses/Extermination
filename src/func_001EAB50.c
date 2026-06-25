// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Screen color-fade overlay: when a global fade timer (D_00275C34+0x54) is below
// 0.5, packs an RGBA color from the fade ramp into 0x70003600..0x70003608 and issues
// a draw call, then runs two follow-up renderer calls unconditionally.
int float_to_int(float);
int func_001CD520(int, int, char *, long long, int, float, float, float);
int func_001CFB50(char *, int, int, float, float, float, float, float);
int func_001CFBE0(int, int, char *, char *, int);
extern char D_00255590[64];
extern char *D_00275C34;
extern char D_0081F8F0[64];

static inline long long shl64(long long x, int n) {
    return x << n;
}

void func_001EAB50(int arg0, int arg1) {
    char *base;
    float a;
    float scl;
    float t192;
    float fade;
    int r;
    int g;
    int b;

    base = D_00275C34 + 0x54;
    a = *(float *)(D_00275C34 + 0x54);
    if (a < 0.5f) {
        scl = (0.5f - a) / 0.5f;
        r = float_to_int(255.0f * scl);
        *(int *)0x70003600 = r;
        t192 = 192.0f * scl;
        g = float_to_int(t192);
        *(int *)0x70003604 = g << 8;
        b = float_to_int(t192);
        *(int *)0x70003608 = b << 16;
        fade = 1.0f + ((4.0f * *(float *)base) / 0.5f);
        func_001CD520(0, 2, (char *)(arg0 + 0x30),
                      (shl64((long long)0x20045B25, 0x20) | shl64(0x9942, 0x10)) | 0x1E98,
                      (((*(int *)0x70003608) | 0x80000000) | (*(int *)0x70003604)) | (*(int *)0x70003600),
                      fade, fade, 2.0f);
    }
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54),
                  *(float *)(D_00275C34 + 0x5C), 1.0f, 9.999999974752427e-07f, 3.0f);
    func_001CFBE0(arg1, 0, D_00255590, D_0081F8F0, 0);
}
