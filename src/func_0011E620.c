// COMPILER: eegcc
// CFLAGS: -O2

extern float func_0011C4C8(float y, float x);
extern int func_0011E080(float x);
extern double func_00128350(float x);
extern int func_0011DB90(void *p);
extern int *func_0011FD78(void);
extern float func_00127758(double h);

extern int D_0026C5D0;
extern int D_0026C640;

float func_0011E620(float y, float x) {
    int mode;
    float saved;
    struct {
        int a;        /* 0x00 */
        int *b;       /* 0x04 */
        double c;     /* 0x08 */
        double d;     /* 0x10 */
        double e;     /* 0x18 */
        int f;        /* 0x20 */
    } st;

    saved = func_0011C4C8(y, x);
    mode = D_0026C5D0;
    if (mode != -1 && func_0011E080(x) == 0 && func_0011E080(y) == 0) {
        if (x == 0.0f && y == 0.0f) {
            st.c = func_00128350(y);
            st.d = func_00128350(x);
            st.e = 0.0;
            st.a = 1;
            st.b = &D_0026C640;
            st.f = 0;
            if (mode == 2 || func_0011DB90(&st) == 0) {
                *func_0011FD78() = 0x21;
            }
            if (st.f != 0)
                *func_0011FD78() = st.f;
            return func_00127758(st.e);
        }
    }
    return saved;
}
