// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00823580 (splat/link name 00823540; overlay code is
// linked 0x40 below where it runs), 0x214 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: state 1 by the short at +0x94 (0xA..0xE) picks a count/size pair (6/4,
//  6/5, 8/7, 8/8, 10/12, rate 0.2), spawns two class 0x8000003B objects at
//  self + 0xD0 (+5 = 0, +0x1F0 = count, +0x1F4 = size, +0x1F8 = 0.2),
//  transforms (0, 0, size, 1) by the matrix, builds a matrix from the negated
//  rows (func_00103230 x3, func_00102948) and calls func_001EFEB0(0, matrix)
//  and func_001EFEB0(0, self + 0xD0). States 0/2/3 nothing.
extern unsigned char *func_001EFEB0(int id, void *m);
extern void func_001026A0(void *dst, void *m, void *v);
extern void func_001029C0(void *m);
extern void func_00103230(void *dst, void *src, float f);
extern void func_00102948(void *dst, void *src);

void overlay_AREA16_func_00823540(unsigned char *self) {
    float v[4];
    float m[4][4];
    float s;
    float r;
    int n;
    int i;
    unsigned char *o;
    switch (self[4]) {
    case 0:
        break;
    case 1:
        switch (*(short *)(self + 0x94)) {
        case 0xA:
            s = 4.0f;
            r = 0.2f;
            n = 6;
            break;
        case 0xB:
            s = 5.0f;
            r = 0.2f;
            n = 6;
            break;
        case 0xC:
            s = 7.0f;
            r = 0.2f;
            n = 8;
            break;
        case 0xD:
            s = 8.0f;
            r = 0.2f;
            n = 8;
            break;
        case 0xE:
            s = 12.0f;
            n = 10;
            r = 0.2f;
            break;
        }
        for (i = 0; i < 2; i++) {
            o = func_001EFEB0(0x8000003B, self + 0xD0);
            if (o != 0) {
                o[5] = 0;
                *(int *)(o + 0x1F0) = n;
                *(float *)(o + 0x1F4) = s;
                *(float *)(o + 0x1F8) = r;
            }
        }
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = s;
        v[3] = 1.0f;
        func_001026A0(v, self + 0xD0, v);
        func_001029C0(m);
        func_00103230(m[0], self + 0xD0, -1.0f);
        func_00103230(m[1], self + 0xE0, -1.0f);
        func_00103230(m[2], self + 0xF0, -1.0f);
        func_00102948(m[3], v);
        func_001EFEB0(0, m);
        func_001EFEB0(0, self + 0xD0);
        break;
    case 2:
    case 3:
        break;
    }
}
