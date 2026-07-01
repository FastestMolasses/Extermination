// NEARMISS func_00197490  (vram 0x00197490, 0x2AC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.04% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS: 3-arg outer-switch on arg1+0x230 (an animation/anim-event id) dispatching to a shared body that sets up world/local transforms via func_00102948/func_001029C0/func_00102C58/func_001026A0 and calls func_001916C0/func_001B1240/func_001B0300. Fully recovered logic verified line-for-line ag...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001026A0(void *, void *, void *);
extern void func_00102948(void *, void *, ...);
extern void func_001029C0(void *);
extern void func_00102C58(void *, void *, void *);
extern void func_00191210(void);
extern void func_001912B0(char *);
extern void func_001916C0(char *, char *, int);
extern void func_00197870(char *, char *, int);
extern void func_00198440(int);
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
    char *s0 = (char *) (long) arg2;
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
        *(unsigned char *)(s2 + 6) = 1;
        *(unsigned char *)(s2 + 1) = 2;
        func_00197870(s2, s1, 1);
        return;
    case 0xD:
        *(unsigned char *)(s2 + 6) = 1;
        *(unsigned char *)(s2 + 1) = 2;
        func_00197870(s2, s1, 1);
        return;
    case 0xC:
        *(unsigned char *)(s2 + 6) = 2;
        *(unsigned char *)(s2 + 1) = 2;
        func_00198440(1);
        func_001912B0(s1);
        goto ret_clear;
    case 0x29:
        *(unsigned char *)(s2 + 6) = 2;
        *(unsigned char *)(s2 + 1) = 2;
        func_00198440(1);
        goto ret_clear;
    }
    if (*(unsigned char *)(s2 + 0x8B) == 0) {
        func_00102948(s1 + 0xA0, &D_70003040, 1);
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
    *(float *)(s2 + 0x10) = *(float *)(s2 + 0x20) + *(float *)(s2 + 0x10);
    *(float *)(s2 + 0x14) = *(float *)(s2 + 0x14) + (*(float *)(s2 + 0x24) + *(float *)(s2 + 0x5C));
    *(float *)(s2 + 0x18) = *(float *)(s2 + 0x28) + *(float *)(s2 + 0x18);
    func_00102948(&D_008105E0, s2 + 0x20);
    func_00102948(&D_008105D0, s2 + 0x10);
    func_00191210();
    *(unsigned char *)(s2 + 6) = 0;
    goto ret_body;
skip_a:
    if (func_0019A910(s2 + 0x10, s2 + 0x20, 6) != 0) {
        func_00102948(s2 + 0x20, &D_700031B0, 2);
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
ret_clear:
    *(unsigned char *)(s2 + 2) = 0;
    *(unsigned char *)(s2 + 3) = 0;
    *(short *)(s2 + 8) = 0;
}
