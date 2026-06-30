// CFLAGS: -O4,p -sdatathreshold 0
extern int func_001026A0(char *a0, void *a1, void *a2);
extern void func_00102948(void *a0, void *a1);
extern void func_001029C0(void *a0);
extern void func_00102C58(void *a0, void *a1, char *a2);
extern void func_001031E0(void *a0, void *a1);
extern void func_0018C4B0(void *p, float a, float b);
extern void func_0018C6A0(char *p, void *a1, float b);
extern char D_008105E0;
extern int D_70003400;
extern int D_70003430;
extern int D_70003600;
extern int D_70003B50;

void func_00197740(char *arg0, char *arg1) {
    func_00102948(arg0 + 0x30, &D_70003B50);
    func_001029C0(&D_70003400);
    func_00102C58(&D_70003400, &D_70003400, arg0 + 0x30);
    *(int *)0x70003600 = 0;
    *(int *)0x70003604 = 0x41980000;
    *(int *)0x70003608 = 0x40C00000;
    *(int *)0x7000360C = 0x3F800000;
    func_001031E0(&D_70003430, arg1 + 0xA0);
    func_001026A0(arg0 + 0x20, &D_70003400, &D_70003600);
    *(int *)0x70003600 = 0;
    *(int *)0x70003604 = 0x41980000;
    *(int *)0x70003608 = 0xC1F00000;
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(arg0 + 0x10, &D_70003400, &D_70003600);
    func_0018C4B0(&D_008105E0, *(float *)(arg0 + 0x24), 0.4f);
    func_0018C6A0(arg0 + 0x20, &D_008105E0, 0.4f);
}
