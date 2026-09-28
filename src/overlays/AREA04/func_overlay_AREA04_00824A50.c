// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00824A90 (splat/link name 00824A50;
// overlay code is linked 0x40 below where it runs), 0x330 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: script callback (self, st, prm). Picks one of four 16-byte points
// (0x828BA0/B0/C0/D0) by +0x2EC against 215/260/305. st +4 0 copies the
// point and +0x2E0 to D_008105D0/D_008105E0, D_008101E0 +0x10/+0x20 and st
// +0x10/+0x20, prm +0x10 = 0; st +4 1 steps prm +0x10 to prm +0xC and
// moves D_008105D0 from st +0x10 toward the midpoint and D_008105E0 from
// st +0x20 toward +0x2E0 by t = prm +0x10 / prm +0xC, copying both into
// D_008101E0 +0x10/+0x20. Returns 1 when done, else 0.
typedef struct { float v[4]; } Vec4 __attribute__((aligned(16)));
extern Vec4 D_overlay_AREA04_00828BA0;
extern Vec4 D_overlay_AREA04_00828BB0;
extern Vec4 D_overlay_AREA04_00828BC0;
extern Vec4 D_overlay_AREA04_00828BD0;
extern char D_008101E0[];
extern float D_008105D0[4];
extern float D_008105E0[4];
extern void func_00102948(void *dst, void *src);
extern void func_001028D0(void *dst, void *a, void *b);

int func_overlay_AREA04_00824A50(unsigned char *self, unsigned char *st, char *prm) {
    Vec4 a, b, c, d;
    Vec4 mid;
    float *pos = (float *)(self + 0x2E0);
    float *sel = a.v;
    char *cam;
    float t;

    a = D_overlay_AREA04_00828BA0;
    b = D_overlay_AREA04_00828BB0;
    c = D_overlay_AREA04_00828BC0;
    d = D_overlay_AREA04_00828BD0;
    cam = D_008101E0;

    if (pos[3] < 260.0f) {
        if (pos[3] < 215.0f) {
            sel = a.v;
        } else {
            sel = b.v;
        }
    } else if (pos[3] < 305.0f) {
        sel = c.v;
    } else {
        sel = d.v;
    }
    switch (st[4]) {
    case 0:
        func_00102948(D_008105D0, sel);
        func_00102948(D_008105E0, pos);
        func_00102948(cam + 0x10, sel);
        func_00102948(cam + 0x20, pos);
        func_00102948(st + 0x10, sel);
        func_00102948(st + 0x20, pos);
        *(float *)(prm + 0x10) = 0.0f;
        st[4] = 1;
        break;
    case 1:
        if (!(*(float *)(prm + 0x10) < *(float *)(prm + 0xC))) {
            return 1;
        }
        *(float *)(prm + 0x10) += 1.0f;
        mid.v[0] = 0.5f * (sel[0] + pos[0]);
        mid.v[1] = 0.5f * (sel[1] + pos[1]);
        mid.v[2] = 0.5f * (sel[2] + pos[2]);
        mid.v[3] = 0.5f * (sel[3] + pos[3]);
        t = *(float *)(prm + 0x10) / *(float *)(prm + 0xC);
        func_001028D0(D_008105D0, &mid, st + 0x10);
        func_001028D0(D_008105E0, pos, st + 0x20);
        *(float *)(0x8105D0) = *(float *)(st + 0x10) + *(float *)(0x8105D0) * t;
        *(float *)(0x8105D4) = *(float *)(st + 0x14) + *(float *)(0x8105D4) * t;
        *(float *)(0x8105D8) = *(float *)(st + 0x18) + *(float *)(0x8105D8) * t;
        *(float *)(0x8105E0) = *(float *)(st + 0x20) + *(float *)(0x8105E0) * t;
        *(float *)(0x8105E4) = *(float *)(st + 0x24) + *(float *)(0x8105E4) * t;
        *(float *)(0x8105E8) = *(float *)(st + 0x28) + *(float *)(0x8105E8) * t;
        func_00102948(cam + 0x10, D_008105D0);
        func_00102948(cam + 0x20, D_008105E0);
        break;
    }
    return 0;
}
