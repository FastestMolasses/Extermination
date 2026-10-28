// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.87% NEARMISS).
// Fixes read from the original instructions:
//  - The anchor components D_008105D0/D4/D8 are declared volatile (the same
//    matching device as the byte-matched func_001B8FC0): the original loads
//    them strictly in x,y,z order and stores each delta in source order;
//    without volatile mwcc 2.3.3 reorders the three anchor loads. This was
//    not a scheduling wall.
//
// In-cone visibility / range test. Writes the delta of the caller's (x,y,z)
// against an anchor (D_008105D0..D8) into the EE scratchpad workspace at
// 0x70003600 (with +0xC = 1.0f), takes the distance via func_0011E748(sumsq)
// (sqrt-like). If distance > 350 -> 0. Else orients the scratchpad vector
// (func_00102760) and dots it against a reference direction D_00810600
// (func_00102738 -> d): closer than 35 always passes; 35..45 passes when
// d>=0; beyond 45 passes only when d>=0.7. Note the func_001B15D0 idiom: the
// calls pass the symbol D_70003600 while the float accesses use the raw
// literal 0x70003600.
extern float func_0011E748(float);
extern void func_00102760(float *, float *);
extern float func_00102738(float *, float *);
extern volatile float D_008105D0;
extern volatile float D_008105D4;
extern volatile float D_008105D8;
extern float D_00810600;
extern float D_70003600[4];

int func_001B1630(float fx, float fy, float fz) {
    float *p = (float *)0x70003600;
    float d;
    float r;
    float a, b, c;
    a = fx - D_008105D0;
    b = fy - D_008105D4;
    c = fz - D_008105D8;
    p[0] = a;
    p[1] = b;
    p[2] = c;
    *(int *)0x7000360C = 0x3F800000;
    r = func_0011E748((p[0] * p[0]) + (p[1] * p[1]) + (p[2] * p[2]));
    if (!(r <= 350.0f)) {
        return 0;
    }
    func_00102760(D_70003600, D_70003600);
    d = func_00102738(D_70003600, &D_00810600);
    if (r < 35.0f) {
        return 1;
    }
    if (r < 45.0f) {
        if (d < 0.0f) {
            return 0;
        }
    } else if (d < 0.7f) {
        return 0;
    }
    return 1;
}
