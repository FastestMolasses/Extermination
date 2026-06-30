// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Per-vertex ring/loop emitter. Caches the current scratch buffer from the
// global render context D_00275670 (+0x1c), arms three pipeline-state calls
// (func_001D1F80/func_001D7510/func_001D1FF0), then for 64 steps sweeps an
// angle from -pi..+pi and emits a vertex displaced from the base point p by
// (fx*sin(ang), fy*cos(ang)) via func_001E0750. Finally writes a 0x60 tag /
// clears the count+flags at the context buffer, advances it by 0x10, and
// hands off to func_001CAAC0.
extern void func_001D1F80(int, int, int);
extern void func_001D7510(int, int, int);
extern void func_001D1FF0(int, int);
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern void func_001E0750(float *, int);
extern void func_001CAAC0(char *, char *, char *);

typedef struct { char pad[0x1c]; char *cur; } Ctx;
extern Ctx *D_00275670;

void func_001E0AC0(char *p, int arg1, float fx, float fy) {
    float v[4];
    char *buf;
    int i;
    float ang;
    Ctx *c;

    buf = D_00275670->cur;
    func_001D1F80(3, 2, 1);
    func_001D7510(3, 0, 0);
    func_001D1FF0(3, 2);
    for (i = 0; i < 0x40; i++) {
        float a, b;
        ang = ((2.0f * (3.1415927f * (float) i)) / 64.0f) - 3.1415927f;
        a = fx * func_0011DE90(ang);
        b = fy * func_0011E2A8(ang);
        v[0] = *(float *)(p + 0) + a;
        v[1] = *(float *)(p + 4) + b;
        v[2] = *(float *)(p + 8);
        v[3] = *(float *)(p + 0xC);
        func_001E0750(v, arg1);
    }
    c = D_00275670;
    *(char *)(c->cur + 3) = 0x60;
    *(int *)(c->cur + 4) = 0;
    *(short *)(c->cur + 0) = 0;
    c->cur = c->cur + 0x10;
    func_001CAAC0(p, buf, (char *)c);
}
