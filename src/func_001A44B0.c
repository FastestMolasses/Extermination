// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003190 0x70003194 0x70003198 0x700031A4
extern float D_70003190;
extern float D_70003194;
extern float D_70003198;
extern float D_700031A4;

int func_001A44B0(short *rec) {
    float *c = (float *)(rec + 2);
    float r;
    float lo;
    float hi;
    float y;
    float dx;
    float dz;
    int down;
    float a;
    float b;

    if (rec[0] & 0x8000) {
        r = c[3];
    } else {
        r = c[4];
    }
    a = D_70003194;
    b = D_700031A4;
    if (a > b) {
        lo = b;
        down = 0;
        hi = a;
    } else {
        lo = a;
        down = 1;
        hi = b;
    }
    if (down) {
        y = c[1] - r;
        if (y <= lo || !(y < hi)) return 0;
    } else {
        y = c[1] + r;
        if (y <= lo || !(y < hi)) return 0;
    }
    dx = D_70003190 - c[0];
    dz = D_70003198 - c[2];
    if (!(c[3] * c[3] < dx * dx + dz * dz)) {
    *(float *)0x700031B0 = D_70003190;
    *(int *)0x700030DC = 0;
    *(int *)0x700030D4 = 0;
    *(float *)0x700031B8 = D_70003198;
    if (down) {
        y = c[1] - r;
        *(short *)0x700030CA = -0x8000;
        *(float *)0x700030D8 = -1.0f;
        *(float *)0x700031B4 = y;
    } else {
        y = c[1] + r;
        *(short *)0x700030CA = 0x4000;
        *(float *)0x700030D8 = 1.0f;
        *(float *)0x700031B4 = y;
    }
        return 1;
    } else {
        return 0;
    }
}
