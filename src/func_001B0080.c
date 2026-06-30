// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Float setup/update for an entity transform block. When the two global gates
// D_00810700==1 && D_00810702==4: hard-set the 0x10..0x2C float fields to fixed
// constants and run func_00102948 on the +0x20 and +0x10 sub-blocks. Otherwise
// run an update path: func_00102948(+0x20), +0x24 += 17.0f, func_00102948(+0x30),
// +0x34 = func_001B1470(+0x34); seed scratchpad 0x70003600 block; func_001026A0;
// accumulate +0x10/+0x14/+0x18 from +0x20/+0x24(+fparg0)/+0x28; finish with two
// func_00102948 calls.
extern void func_00102948(void *, void *);
extern void func_001026A0(char *, void *, void *);
extern float func_001B1470(float);
extern void func_001029C0(void *);
extern void func_00102C58(void *, void *, char *);
extern unsigned char D_00810700;
extern unsigned char D_00810702;
extern int D_008105D0;
extern int D_008105E0;
extern int D_00810350;
extern int D_70003400;
extern int D_70003600;
extern int D_70003B50;

void func_001B0080(char *arg0, float fparg0) {
    if (D_00810700 == 1 && D_00810702 == 4) {
        *(float *)(arg0 + 0x10) = -4.3f;
        *(float *)(arg0 + 0x14) = 21.7f;
        *(float *)(arg0 + 0x18) = -572.5f;
        *(int *)(arg0 + 0x1C) = 0x3F800000;
        *(float *)(arg0 + 0x20) = 39.7f;
        *(float *)(arg0 + 0x24) = 16.1f;
        *(float *)(arg0 + 0x28) = -557.5f;
        *(int *)(arg0 + 0x2C) = 0x3F800000;
        func_00102948(&D_008105E0, arg0 + 0x20);
        func_00102948(&D_008105D0, arg0 + 0x10);
        return;
    }
    func_00102948(arg0 + 0x20, &D_00810350);
    *(float *)(arg0 + 0x24) += 17.0f;
    func_00102948(arg0 + 0x30, &D_70003B50);
    *(float *)(arg0 + 0x34) = func_001B1470(*(float *)(arg0 + 0x34));
    func_001029C0(&D_70003400);
    func_00102C58(&D_70003400, &D_70003400, arg0 + 0x30);
    *(int *)0x70003600 = 0;
    *(int *)0x70003604 = 0;
    *(float *)0x70003608 = *(float *)(arg0 + 0xC);
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(arg0 + 0x10, &D_70003400, &D_70003600);
    *(float *)(arg0 + 0x10) += *(float *)(arg0 + 0x20);
    *(float *)(arg0 + 0x14) += *(float *)(arg0 + 0x24) + fparg0;
    *(float *)(arg0 + 0x18) += *(float *)(arg0 + 0x28);
    func_00102948(&D_008105E0, arg0 + 0x20);
    func_00102948(&D_008105D0, arg0 + 0x10);
}
