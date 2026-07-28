// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Seeds the scratch vec at 0x700038A0 from the entity at arg0->0x1C
// (+0xB0/B4/B8, w = 1.0f), runs func_00102948 over table D_008105E0, then
// recomputes the vec from camera params: x = D_00810360 - 15*func_0011E2A8(D_00810374),
// y = 5 + D_00810364, z = D_00810368 - 15*func_0011DE90(D_00810374), and runs
// func_00102948 again over D_008105D0. Returns 1.
// (The `f` local re-reads D_00810374 for the second trig call — matches the
// target's early fa0 load; value is identical.)
extern int func_00102948();
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern float D_00810360;
extern float D_00810364;
extern float D_00810368;
extern float D_00810374;
extern char D_008105D0[];
extern char D_008105E0[];
extern char D_700038A0[];

int func_001BB310(char *arg0) {
    char *p;
    float t0, t1, f;

    p = *(char **)(arg0 + 0x1C);
    *(float *)0x700038A0 = *(float *)(p + 0xB0);
    *(float *)0x700038A4 = *(float *)(p + 0xB4);
    *(float *)0x700038A8 = *(float *)(p + 0xB8);
    *(float *)0x700038AC = 1.0f;
    func_00102948(D_008105E0, D_700038A0);
    t0 = D_00810360 - (15.0f * func_0011E2A8(D_00810374));
    t1 = 5.0f + D_00810364;
    f = D_00810374;
    *(float *)0x700038A0 = t0;
    *(float *)0x700038A4 = t1;
    *(float *)0x700038A8 = D_00810368 - (15.0f * func_0011DE90(f));
    func_00102948(D_008105D0, D_700038A0);
    return 1;
}
