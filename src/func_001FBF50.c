// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Approach-and-retreat distance driver: measures the distance from the actor at
// arg0+0xB0 to the reference transform, returns 0 when out of fparg0 range, else
// writes a cosine-eased magnitude (split by func_001B1380's side test) to *arg1/*arg2.

extern int float_to_int(float);
extern void func_001026A0(float *, float *, float *);
extern float func_00102738(float *, float *);
extern void func_00102760(float *, float *);
extern void func_001028D0(float *, float *, float *);
extern void func_00102948(float *, float *);
extern void func_001029C0(float *);
extern void func_00102C58(float *, float *, float *);
extern float func_0011E2A8(float);
extern int func_001B1380(char *, float *, float);
extern float func_001B15D0(float *, float *);
extern unsigned char D_0028215B;
extern float D_0081027C;
extern float D_00810360;
extern float D_008105D0;
extern float D_70003400;
extern float D_70003600;
extern float D_70003610;

int func_001FBF50(char *arg0, int *arg1, int *arg2, char arg3, float fparg0, float fparg1) {
    float *q;
    float dist;
    float scaled;
    float k;
    float t;
    float p;
    float w;
    int iv;

    *arg2 = 0;
    *arg1 = 0;
    func_00102948(&D_70003600, &D_00810360);
    func_00102948(q = &D_70003610, (float *)(arg0 + 0xB0));
    if (arg3 != 0) {
        *(int *)0x70003614 = 0;
        *(int *)0x70003604 = 0;
    }
    dist = func_001B15D0(&D_70003600, &D_70003610);
    if (!(dist < fparg0)) {
        return 0;
    }
    scaled = fparg1 * func_0011E2A8(1.5707964f * ((fparg0 - dist) / fparg0));
    func_001029C0(&D_70003400);
    *(int *)0x70003600 = 0;
    *(float *)0x70003604 = D_0081027C;
    *(int *)0x70003608 = 0;
    *(int *)0x7000360C = 0x3F800000;
    func_00102C58(&D_70003400, &D_70003400, &D_70003600);
    *(int *)0x70003600 = 0;
    *(int *)0x70003604 = 0;
    *(int *)0x70003608 = 0x3F800000;
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(&D_70003600, &D_70003400, &D_70003600);
    *(float *)0x70003610 = *(float *)(arg0 + 0xB0);
    *(float *)0x70003614 = *(float *)(arg0 + 0xB4);
    *(float *)0x70003618 = *(float *)(arg0 + 0xB8);
    *(float *)0x7000361C = *(float *)(arg0 + 0xBC);
    func_001028D0(&D_70003610, &D_70003610, &D_008105D0);
    *(int *)0x70003614 = 0;
    func_00102760(&D_70003610, &D_70003610);
    if (dist <= 18.0f) {
        w = 0.055555556f * dist;
    } else {
        w = 1.0f;
    }
    if (D_0028215B == 1) {
        iv = float_to_int(scaled);
        *arg2 = iv;
        *arg1 = iv;
        return 1;
    }
    t = func_00102738(&D_70003600, &D_70003610);
    p = t * t;
    p = p * p;
    k = t * p;
    k = k * w;
    if (k < 0.0f) {
        k = k - (1.0f - w);
    } else {
        k = k + (1.0f - w);
    }
    if (func_001B1380((char *)(arg0 + 0xB0), &D_008105D0, D_0081027C) != 0) {
        *arg1 = float_to_int(scaled);
        *arg2 = float_to_int(scaled * k);
    } else {
        *arg2 = float_to_int(scaled);
        *arg1 = float_to_int(scaled * k);
    }
    return 1;
}
