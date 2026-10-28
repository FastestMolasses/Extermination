// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern int func_0019AD00(char *obj, float *v, int flags);
extern int func_001B2D00(float *v, float *ground);
extern int func_001B2F70(char *v, float *ground);

int func_001B2B80(char *obj, float *v, float *ground) {
    v[3] = 1.0f;
    if (func_0019AD00(obj, v, 7) != 0) {
        return func_001B2D00(v, ground) | 4;
    }
    return func_001B2F70((char *)v, ground);
}
