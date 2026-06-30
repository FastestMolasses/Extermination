// NEARMISS func_0011E420  (vram 0x0011E420, 0xFC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.97% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler wall. Body decoded fully (64/64 instrs, correct semantics): float wrapper around func_0011BCF8; on mode!=-1 && func_0011E080(x)==0 && 1.0f<func_0011DF78(x) it builds a 6-field stack struct, calls func_00128350(x)/func_0011DB90/func_0011FD78(errno=0x21)/func_00127758, returns saved. Two residual ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern float func_0011BCF8(float x);
extern int func_0011E080(float x);
extern float func_0011DF78(float x);
extern long func_00128350(float x);
extern int func_0011DB90(void *p);
extern int *func_0011FD78(void);
extern int func_00127758(long h);

extern int D_0026C5D0;
extern int D_0026C630;

float func_0011E420(float x) {
    int mode;
    float saved;
    struct {
        int a;        /* 0x00 */
        int *b;       /* 0x04 */
        long c;       /* 0x08 */
        long d;       /* 0x10 */
        long e;       /* 0x18 */
        int f;        /* 0x20 */
    } st;

    saved = func_0011BCF8(x);
    mode = D_0026C5D0;
    if (mode != -1 && func_0011E080(x) == 0 && 1.0f < func_0011DF78(x)) {
        st.a = 1;
        st.b = &D_0026C630;
        st.f = 0;
        st.c = func_00128350(x);
        st.d = st.c;
        st.e = 0;
        if (mode == 2 || func_0011DB90(&st) == 0) {
            *func_0011FD78() = 0x21;
        }
        if (st.f != 0)
            *func_0011FD78() = st.f;
        func_00127758(st.e);
        return saved;
    }
    return saved;
}
