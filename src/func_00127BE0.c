// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): convert two floats, flip a flag, then combine.
extern void func_001278C0(float *in, void *out);
extern int func_00127950(void *a, void *b, void *out);
extern int func_001277B0(int p);

int func_00127BE0(float x, float y) {
    char out0[0x10];
    int out10[4];
    char out20[0x10];
    float fx;
    float fy;
    fx = x;
    fy = y;
    func_001278C0(&fx, out0);
    func_001278C0(&fy, out10);
    out10[1] ^= 1;
    return func_001277B0(func_00127950(out0, out10, out20));
}
