// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001026A0(void *, void *, int);
extern void func_001028B8(void *, void *, void *);
extern void func_001029C0(void *);
extern void func_00102BB0(void *, void *, float);
extern float func_0011E620(float, float);
extern void func_0019B4C0(void *, void *, void *, unsigned int);
extern float func_001B1470(float);

extern char D_70003440;
extern char D_700038C0;
extern char D_700038D0;

void func_001C2690(char *arg0, int arg1, int arg2) {
    func_001029C0(&D_70003440);
    func_00102BB0(&D_70003440, &D_70003440,
                  func_001B1470(func_0011E620(*(float *)(arg0 + 0x70),
                                              *(float *)(arg0 + 0x78))));
    func_001026A0(&D_700038C0, &D_70003440, arg1);
    func_001028B8(&D_700038C0, &D_700038C0, arg0 + 0xB0);
    func_001026A0(&D_700038D0, &D_70003440, arg2);
    func_0019B4C0(arg0, &D_700038C0, &D_700038D0, 0x80000006);
}
