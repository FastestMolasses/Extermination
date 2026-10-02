// COMPILER: eegcc
// CFLAGS: -O2
// Byte-matched (lane DFIX2 2026-10-02, was NEARMISS 91.97%) and corrected against the
// original instructions, as its twin func_0011E420 (docs/FINDINGS.md "NEARMISS body
// corrections from the level side-track lanes, second round"): on the domain-error path
// the result is func_00127758(st.e), the exception record's double value converted to
// float; the old C returned the kernel's value on every path.
// libm wrapper: kernel func_0011C128(x), exception name D_0026C638; otherwise the same
// shape as func_0011E420.

extern float func_0011C128(float x);
extern int func_0011E080(float x);
extern float func_0011DF78(float x);
extern double func_00128350(float x);
extern int func_0011DB90(void *p);
extern int *func_0011FD78(void);
extern float func_00127758(double h);

extern int D_0026C5D0;
extern int D_0026C638;

float func_0011E520(float x) {
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

    saved = func_0011C128(x);
    mode = D_0026C5D0;
    if (mode != -1 && func_0011E080(x) == 0 && 1.0f < func_0011DF78(x)) {
        st.a = 1;
        st.b = &D_0026C638;
        st.f = 0;
        st.d = func_00128350(x);
        st.c = st.d;
        st.e = 0.0;
        if (mode == 2 || func_0011DB90(&st) == 0) {
            *func_0011FD78() = 0x21;
        }
        if (st.f != 0)
            *func_0011FD78() = st.f;
        return func_00127758(st.e);
    }
    return saved;
}
