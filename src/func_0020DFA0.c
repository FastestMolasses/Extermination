// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001AFE60(void);
extern void func_0020E020(void);
extern void func_001029C0(void *p);
extern void func_0021BAC0(int a0);
extern void func_0021B9A0(int a0, float f12, float f13);
extern void func_001D2610(float f12);
extern char D_00810610[];
extern float D_00810624;

void func_0020DFA0(void) {
    func_001AFE60();
    func_0020E020();
    func_001029C0(D_00810610);
    D_00810624 *= -1.0f;
    func_0021BAC0(0);
    func_0021B9A0(5, 0.0f, 1000000.0f);
    func_001D2610(0.0f);
}
