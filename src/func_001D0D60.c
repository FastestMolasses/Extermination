// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (lane DMATCH 2026-10-02, was companion C at 94.32%): declaring w before frac
// gives the original's float register colouring (fraction in f21, the weight 1 - fraction
// in f20).
// Advances an animation track: time (a+8) += dt. Looping (a+0xC set): while time
// >= length (a+4) it subtracts length and flags 0x1000; next = float_to_int(time + 1)
// wraps to 0 at the length. Clamped: time >= length gives time = length - 1 and the
// flag; next = float_to_int(time + 1), clamped to float_to_int(length - 1). Then
// frac = time - float_to_int(time), and the 7 floats at a+0x40 become
// (1 - frac) * key[cur][i] + frac * key[next][i] with keys of 7 floats at *(a+0),
// cur = float_to_int(time). Returns the flag.
extern int float_to_int(float f);

int func_001D0D60(char *a, float dt) {
    int flag;
    int next;
    int cur;
    int i;
    float t;
    float len;
    float w;
    float frac;
    float *base;
    float *q;
    float *p;

    *(float *)(a + 8) += dt;
    flag = 0;
    if (*(unsigned char *)(a + 0xC)) {
        while (!((t = *(float *)(a + 8)) < (len = *(float *)(a + 4)))) {
            flag = 0x1000;
            *(float *)(a + 8) -= len;
        }
        next = float_to_int(t + 1.0f);
        if (!((float)next < len)) {
            next = 0;
        }
    } else {
        if (!(*(float *)(a + 8) < *(float *)(a + 4))) {
            flag = 0x1000;
            *(float *)(a + 8) = *(float *)(a + 4) - 1.0f;
        }
        next = float_to_int(*(float *)(a + 8) + 1.0f);
        if (!((float)next < *(float *)(a + 4))) {
            next = float_to_int(*(float *)(a + 4) - 1.0f);
        }
    }
    t = *(float *)(a + 8);
    frac = t - (float)float_to_int(t);
    base = *(float **)(a + 0);
    w = 1.0f - frac;
    cur = float_to_int(t);
    p = base + cur * 7;
    q = base + next * 7;
    for (i = 0; i < 7; i++) {
        *(float *)(a + 0x40) = w * *p + frac * *q;
        q++;
        p++;
        a += 4;
    }
    return flag;
}
