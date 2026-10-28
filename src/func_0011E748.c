// COMPILER: eegcc
// CFLAGS: -O2
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 92.58% NEARMISS).
// Fixes read from the original instructions:
//  - the exception record is a struct (as in the byte-matched twins func_0011E420 /
//    func_0011E620) with double fields; func_00128350 returns a double (float to double)
//    and func_00127758 (double to float) returns a float;
//  - the domain-error path returns func_00127758(st.e), not the kernel's value;
//  - D_0026C650 is a double constant (the non-SVID result) and the mode-2 test re-reads
//    D_0026C5D0 rather than using the cached mode;
//  - the two argument doubles are set in one chained assignment.
// libm wrapper (sqrtf-shaped): kernel func_0011CB90(x); when the error mode D_0026C5D0 is
// not -1, x is not a NaN (func_0011E080) and x < 0, it fills the exception record (type 1,
// name D_0026C648, x as a double twice, result 0.0 in mode 0 else D_0026C650), stores 0x21
// through func_0011FD78's pointer when the mode is 2 or func_0011DB90(record) returns 0,
// stores the record's word +0x20 there when it is nonzero, and returns the record's result
// converted to float. Otherwise it returns the kernel's value.

extern float func_0011CB90(float x);
extern int func_0011E080(float x);
extern double func_00128350(float x);
extern int func_0011DB90(void *p);
extern int *func_0011FD78(void);
extern float func_00127758(double h);

extern int D_0026C5D0;
extern int D_0026C648;
extern double D_0026C650;

float func_0011E748(float x) {
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

    saved = func_0011CB90(x);
    mode = D_0026C5D0;
    if (mode == -1 || func_0011E080(x) != 0)
        return saved;
    if (x < 0.0f) {
        st.a = 1;
        st.b = &D_0026C648;
        st.f = 0;
        st.c = st.d = func_00128350(x);
        if (mode == 0)
            st.e = 0.0;
        else
            st.e = D_0026C650;
        if (D_0026C5D0 == 2 || func_0011DB90(&st) == 0) {
            *func_0011FD78() = 0x21;
        }
        if (st.f != 0)
            *func_0011FD78() = st.f;
        return func_00127758(st.e);
    }
    return saved;
}
