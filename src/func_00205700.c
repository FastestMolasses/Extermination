// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Double-buffer flip of a display environment: the DISPFB frame base (the
// 9-bit FBP field of the register at +0x10) is set to the page at +0x2C when
// which is non-zero, else +0x28; the environment is then pushed to the GS
// with func_00100550.
extern void func_00100550(void *env);

typedef struct DispEnv {
    char pad[0x10];
    unsigned long long fbp : 9;
    unsigned long long fbw : 6;
    unsigned long long psm : 5;
    unsigned long long pad1 : 12;
    unsigned long long dbx : 11;
    unsigned long long dby : 11;
    unsigned long long pad2 : 10;
    char pad18[0x10];
    int page0;  /* 0x28 */
    int page1;  /* 0x2C */
} DispEnv;

void func_00205700(DispEnv *env, int which) {
    int page;

    if (which) {
        page = env->page1;
    } else {
        page = env->page0;
    }
    env->fbp = page;
    func_00100550(env);
}
