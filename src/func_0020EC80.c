// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-entity setup: builds a transform (D_70003400 vector/matrix scratch from
// arg0+0xC0/0xC4/0xC8 angles), composes it with the -1 vector at D_70003440,
// copies the result quad to D_700036A0, then (when global flag D_008104E4==1)
// initializes a secondary transform (D_700038A0) from arg0+0x118 and feeds it
// to func_001F4BF0. Brackets the indirect vtable call at arg0+0x4C with
// func_001D2040(0,1)/(0,0) guards. Stored 32-bit constants are float bit
// patterns (0xBF800000=-1.0, 0x3F800000=1.0, 0x404CCCCD=3.2, etc).
extern int func_001029C0(void *);
extern int func_00102B08(void *, void *, float);
extern int func_00102BB0(void *, void *, float);
extern int func_00102A60(void *, void *, float);
extern int func_001026D0(void *, void *, void *);
extern int func_001026A0(void *, void *, void *);
extern int func_001031E0(void *, void *);
extern int copy_qw4(void *, void *);
extern int func_001C69A0(void *);
extern int func_001D2040(int, int);
extern int func_001F4BF0(void *, void *);

extern char D_70003400;
extern char D_70003430;
extern char D_70003440;
extern char D_700036A0;
extern char D_700038A0;
extern char D_700038B0;
extern unsigned char D_008104E4;

void func_0020EC80(char *arg0) {
    func_001029C0(&D_70003400);
    func_00102B08(&D_70003400, &D_70003400, *(float *)(arg0 + 0xC0));
    func_00102BB0(&D_70003400, &D_70003400, *(float *)(arg0 + 0xC4));
    func_00102A60(&D_70003400, &D_70003400, *(float *)(arg0 + 0xC8));
    func_001029C0(&D_70003440);
    *(int *)0x70003440 = 0xBF800000;
    *(int *)0x70003454 = 0xBF800000;
    *(int *)0x70003468 = 0xBF800000;
    func_001026D0(&D_70003400, &D_70003440, &D_70003400);
    func_00102B08(&D_70003400, &D_70003400, 3.1415927f);
    func_001031E0(&D_70003430, arg0 + 0xB0);
    copy_qw4(&D_700036A0, &D_70003400);
    func_001C69A0(arg0);
    if (D_008104E4 == 1) {
        *(int *)0x700038A0 = 0x404CCCCD;
        *(int *)0x700038A4 = 0xBFC00000;
        *(int *)0x700038A8 = 0xBF19999A;
        *(int *)0x700038AC = 0x3F800000;
        func_001026A0(&D_700038A0, *(char **)(arg0 + 0x118) + 0x90, &D_700038A0);
        *(int *)0x700038B0 = 0x20;
        *(int *)0x700038B4 = 0x70;
        *(int *)0x700038B8 = 0x80;
        *(int *)0x700038BC = 0x80;
        func_001F4BF0(&D_700038A0, &D_700038B0);
    }
    func_001D2040(0, 1);
    (*(void (**)(char *))(arg0 + 0x4C))(arg0);
    func_001D2040(0, 0);
}
