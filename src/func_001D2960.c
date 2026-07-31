// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Per-frame main projection builder: builds P at ctx+0x2340 from the zoom s at ctx+0x2468,
// copies the view matrix arg0 to ctx+0x2380 and composes K = P*V into ctx+0x23C0; then builds
// four alternate (offscreen/shadow/reflection) P*V variants at ctx+0x2240/0x2280/0x22C0/0x2300
// via func_001D2D20(zoom, w, h, near, far); finally writes the guard-band clip constants at
// ctx+0x2220 and the four frustum cull planes at ctx+0x2410/0x2420/0x2430/0x2440
// (normal = (+-s, 0, -1023) / sqrt(s*s + 1023*1023)).

extern void copy_qw4(void *dst, void *src);
extern void func_001026D0(void *dst, void *m, void *src);
extern void func_001D2D20(void *m, float zoom, float w, float h, float near, float far);
extern float func_0011E748(float x);
extern char *D_00275670;

#define CTX ((char *)D_00275670)

void func_001D2960(void *arg0) {
    float s;
    float m[16];
    float norm, neg_s, inv_len, inv_len2;
    int h_i;
    float h;

    s = *(float *)(CTX + 0x2468);
    *(float *)(CTX + 0x2340) = 0.8f * s;
    *(int *)(CTX + 0x2350) = 0;
    *(int *)(CTX + 0x2360) = 0x45000000;
    *(int *)(CTX + 0x2370) = 0;
    *(int *)(CTX + 0x2344) = 0;
    *(float *)(CTX + 0x2354) = 0.5f * s;
    *(int *)(CTX + 0x2364) = 0x45000000;
    *(int *)(CTX + 0x2374) = 0;
    *(int *)(CTX + 0x2348) = 0;
    *(int *)(CTX + 0x2358) = 0;
    *(int *)(CTX + 0x2368) = 0x3F664CB3;
    *(int *)(CTX + 0x2378) = 0x49CCCCCC;
    *(int *)(CTX + 0x234C) = 0;
    *(int *)(CTX + 0x235C) = 0;
    *(int *)(CTX + 0x236C) = 0x3F800000;
    *(int *)(CTX + 0x237C) = 0;

    copy_qw4(CTX + 0x2380, arg0);
    func_001026D0(CTX + 0x23C0, CTX + 0x2340, CTX + 0x2380);

    h_i = 560;
    h = (float)h_i;
    func_001D2D20(m, s, 1280.0f, h, 0.1f, 16711680.0f);
    func_001026D0(CTX + 0x2240, m, CTX + 0x2380);
    func_001D2D20(m, s, 1280.0f, 560.0f, 20.0f, 16711680.0f);
    func_001026D0(CTX + 0x2280, m, CTX + 0x2380);
    func_001D2D20(m, s, 3584.0f, 3584.0f, 0.1f, 16711680.0f);
    func_001026D0(CTX + 0x22C0, m, CTX + 0x2380);
    func_001D2D20(m, s, 2048.0f, 2048.0f, 0.1f, 16711680.0f);
    func_001026D0(CTX + 0x2300, m, CTX + 0x2380);

    *(int *)(CTX + 0x2220) = 0x3A008081;
    *(int *)(CTX + 0x2224) = 0x3A008081;
    *(int *)(CTX + 0x2228) = 0x34000000;
    *(int *)(CTX + 0x222C) = 0x3F800000;
    *(int *)(CTX + 0x2230) = 0xBF808081;
    *(int *)(CTX + 0x2234) = 0xBF808081;
    *(int *)(CTX + 0x2238) = 0xBF7FFFFE;
    norm = 1046529.0f + (s * s);
    *(int *)(CTX + 0x223C) = 0;

    inv_len = 1.0f / func_0011E748(norm);
    *(float *)(CTX + 0x2410) = s * inv_len;
    *(int *)(CTX + 0x2414) = 0;
    *(float *)(CTX + 0x2418) = -(-1023.0f * inv_len);
    *(int *)(CTX + 0x241C) = 0;

    inv_len2 = 1.0f / func_0011E748(norm);
    neg_s = -s;
    *(float *)(CTX + 0x2420) = neg_s * inv_len2;
    *(int *)(CTX + 0x2424) = 0;
    *(float *)(CTX + 0x2428) = -(-1023.0f * inv_len2);
    *(int *)(CTX + 0x242C) = 0;

    inv_len = 1.0f / func_0011E748(norm);
    *(int *)(CTX + 0x2430) = 0;
    *(float *)(CTX + 0x2434) = s * inv_len;
    *(float *)(CTX + 0x2438) = -(-1023.0f * inv_len);
    *(int *)(CTX + 0x243C) = 0;

    inv_len2 = 1.0f / func_0011E748(norm);
    *(int *)(CTX + 0x2440) = 0;
    *(float *)(CTX + 0x2444) = neg_s * inv_len2;
    *(float *)(CTX + 0x2448) = -(-1023.0f * inv_len2);
    *(int *)(CTX + 0x244C) = 0;
}
