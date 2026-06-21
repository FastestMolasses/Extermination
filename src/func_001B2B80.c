// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern int func_0019AD00(int a, int b, int c);
extern int func_001B2D00(int a, int b);
extern int func_001B2F70(int a, int b);

int func_001B2B80(int arg0, int arg1, int arg2) {
    *(float *)(arg1 + 0xC) = 1.0f;
    if (func_0019AD00(arg0, arg1, 7) != 0) {
        return func_001B2D00(arg1, arg2) | 4;
    }
    return func_001B2F70(arg1, arg2);
}
