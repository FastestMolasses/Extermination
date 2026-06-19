// COMPILER: eegcc
// CFLAGS: -O2
// SDK: classify two floats (func_001278C0) then compare results (func_00127FA0).
typedef struct { int w[4]; } Cls;
extern int func_001278C0(float *in, Cls *out);
extern int func_00127FA0(Cls *a, Cls *b);

int func_001280B8(float x, float y) {
    Cls a;
    Cls b;
    float fx = x;
    float fy = y;
    func_001278C0(&fx, &a);
    func_001278C0(&fy, &b);
    return func_00127FA0(&a, &b);
}
