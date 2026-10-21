// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), decomp2 lane round 3 (was a 91.04% NEARMISS).
// Camera release step: arg0 is the camera, arg1 the player, arg2 a flag
// (func_00197D20 passes 1, func_00198650 passes 0). Dispatches on the player
// code at arg1 + 0x230:
//   0x2A / 0xD: camera +6 = 1, +1 = 2, func_00197870(cam, player, 1), return.
//   0xC: +6 = 2, +1 = 2, func_00198440(cam, player, 1), func_001912B0(player),
//        return.
//   0x29: +6 = 2, +1 = 2, func_00198440(cam, player, 1), return.
//   0x18 / 0x17 / 5 skip the +0x8B test; any other code first copies
//   D_70003040 to player + 0xA0 when cam + 0x8B is 0. Then: with arg2 set,
//   func_0019A910(cam + 0x10, cam + 0x20, 6) and, if it returns nonzero,
//   func_00102948(cam + 0x20, &D_700031B0), then func_001916C0(cam, player, 2)
//   and cam + 0xA0 = 0x50; with arg2 clear and cam + 5 == 0, rebuild the eye /
//   target from D_70003B50, the player position and the scratchpad matrix
//   D_70003400 with the offset {0, 0, -10, 1} at D_70003600, copy them to
//   D_008105E0 / D_008105D0 and func_00191210(). Then cam + 6 = 0, cam + 1 = 0,
//   and either cam + 0x44 = func_001B1240(&D_008105D0, ...) (cam + 5 == 0) or
//   func_001B0300(); finally +2, +3 and +8 are cleared.
// Body corrections from the original instructions: cases 0xC and 0x29 return
// straight to the epilogue (the old C also cleared +2 / +3 / +8 there); the
// func_00198440 calls pass (cam, player, 1) (the target leaves both incoming
// arguments in $a0 / $a1 and sets only $a2; the old C passed only the 1);
// func_00102948 takes two pointers (the old C's third arguments were the next
// call's $a2); arg2 is an int flag; 0x2A and 0xD share one body. The two
// accumulates are `+=` (idiom-21).

extern void func_001026A0(void *, void *, void *);
extern void func_00102948(void *, void *);
extern void func_001029C0(void *);
extern void func_00102C58(void *, void *, void *);
extern void func_00191210(void);
extern void func_001912B0(char *);
extern void func_001916C0(char *, char *, int);
extern void func_00197870(char *, char *, int);
extern void func_00198440(float *, int, int);
extern int func_0019A910(char *, char *, int);
extern void func_001B0300(void);
extern float func_001B1240(float *, float, float);
extern float D_008105D0;
extern float D_008105E0;
extern float D_008105E8;
extern int D_70003040;
extern int D_700031B0;
extern int D_70003400;
extern int D_70003600;
extern int D_70003B50;

void func_00197490(char *arg0, char *arg1, int arg2) {
    char *s1 = arg1;
    char *s2 = arg0;
    int s0 = arg2;
    int st = *(int *)(arg1 + 0x230);
    int a2;
    float f;

    if (st == 0x18) {
        goto common;
    }
    if (st == 0x17) {
        goto common;
    }
    if (st == 5) {
        goto common;
    }
    switch (st) {
    case 0x2A:
    case 0xD:
        *(unsigned char *)(s2 + 6) = 1;
        *(unsigned char *)(s2 + 1) = 2;
        func_00197870(s2, s1, 1);
        return;
    case 0xC:
        *(unsigned char *)(s2 + 6) = 2;
        *(unsigned char *)(s2 + 1) = 2;
        func_00198440((float *)s2, (int)s1, 1);
        func_001912B0(s1);
        return;
    case 0x29:
        *(unsigned char *)(s2 + 6) = 2;
        *(unsigned char *)(s2 + 1) = 2;
        func_00198440((float *)s2, (int)s1, 1);
        return;
    }
    if (*(unsigned char *)(s2 + 0x8B) == 0) {
        func_00102948(s1 + 0xA0, &D_70003040);
    }
common:
    if (s0 != 0) {
        goto skip_a;
    }
    if (*(unsigned char *)(s2 + 5) != 0) {
        goto skip_b;
    }
    func_00102948(s2 + 0x30, &D_70003B50);
    func_00102948(s2 + 0x20, s1 + 0xA0);
    *(float *)(s2 + 0x24) = *(float *)(s1 + 0xB4) + *(float *)(s2 + 0x8C);
    func_001029C0(&D_70003400);
    func_00102C58(&D_70003400, &D_70003400, s2 + 0x30);
    *(int *)0x70003600 = 0;
    *(int *)0x70003604 = 0;
    *(int *)0x70003608 = 0xC1200000;
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(s2 + 0x10, &D_70003400, &D_70003600);
    *(float *)(s2 + 0x10) += *(float *)(s2 + 0x20);
    *(float *)(s2 + 0x14) = *(float *)(s2 + 0x14) + (*(float *)(s2 + 0x24) + *(float *)(s2 + 0x5C));
    *(float *)(s2 + 0x18) += *(float *)(s2 + 0x28);
    func_00102948(&D_008105E0, s2 + 0x20);
    func_00102948(&D_008105D0, s2 + 0x10);
    func_00191210();
    *(unsigned char *)(s2 + 6) = 0;
    goto ret_body;
skip_a:
    if (func_0019A910(s2 + 0x10, s2 + 0x20, 6) != 0) {
        func_00102948(s2 + 0x20, &D_700031B0);
    }
    func_001916C0(s2, s1, 2);
    *(short *)(s2 + 0xA0) = 0x50;
skip_b:
    *(unsigned char *)(s2 + 6) = 0;
ret_body:
    if (*(unsigned char *)(s2 + 5) == 0) {
        *(unsigned char *)(s2 + 1) = 0;
        f = func_001B1240(&D_008105D0, D_008105E0, D_008105E8);
        *(float *)(s2 + 0x44) = f;
    } else {
        *(unsigned char *)(s2 + 1) = 0;
        func_001B0300();
    }
    *(unsigned char *)(s2 + 2) = 0;
    *(unsigned char *)(s2 + 3) = 0;
    *(short *)(s2 + 8) = 0;
}
