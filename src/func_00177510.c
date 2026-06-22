// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Snapshots a 3-float vector from scratchpad regs (0x700031B0..B8) into the
// active display block at 0x70003050..58, copies three floats from the object
// pointed to by *(0x700031D0) (offsets +0x24/+0x28/+0x2C) into 0x70003060..68,
// derives an angle = func_001B1470( (3*pi/2) + atan2-like(-*(src+0x2C),
// *(0x70003060)) ), stores it to 0x700031E4, then runs func_001029C0 /
// func_00102BB0 over the gp-rel-addressed work block D_70003070.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build tops
// out at 93.1% (FP register-coloring divergence on the 3 simultaneously-live
// loads). objdiff 100.0% vs expected.
extern float func_0011E620(float dx, float dy);
extern float func_001B1470(float angle);
extern void func_001029C0(void *p);
extern void func_00102BB0(void *a, void *b, float v);
extern char D_70003070[];

void func_00177510(void) {
    float c, b, a;
    float d;
    float angle;
    char *src;

    a = *(volatile float *)0x700031B0;
    b = *(volatile float *)0x700031B4;
    c = *(volatile float *)0x700031B8;
    src = *(char **)0x700031D0;
    *(float *)0x70003050 = a;
    *(float *)0x70003054 = b;
    *(float *)0x70003058 = c;
    *(float *)0x70003060 = *(float *)(src + 0x24);
    *(float *)0x70003064 = *(float *)(src + 0x28);
    d = *(float *)(src + 0x2C);
    *(float *)0x70003068 = d;
    angle = func_0011E620(-d, *(float *)0x70003060);
    angle = func_001B1470(4.71238899230957f + angle);
    *(float *)0x700031E4 = angle;
    func_001029C0(D_70003070);
    func_00102BB0(D_70003070, D_70003070, *(float *)0x700031E4);
}
