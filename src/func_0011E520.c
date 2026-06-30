// NEARMISS func_0011E520  (vram 0x0011E520, 0xFC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.97% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler wall. Exact twin of func_0011E420 (only differs: calls func_0011C128 instead of func_0011BCF8, uses global D_0026C638 instead of D_0026C630). Same two residual clusters: independent sd-store reordering at sp+0x8/0x18/0x10 and duplicated epilogue ld ra. Deterministic ee-gcc 2.9 scheduling/block-l...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern float func_0011C128(float x);
extern int func_0011E080(float x);
extern float func_0011DF78(float x);
extern long func_00128350(float x);
extern int func_0011DB90(void *p);
extern int *func_0011FD78(void);
extern int func_00127758(long h);

extern int D_0026C5D0;
extern int D_0026C638;

float func_0011E520(float x) {
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

    saved = func_0011C128(x);
    mode = D_0026C5D0;
    if (mode != -1 && func_0011E080(x) == 0 && 1.0f < func_0011DF78(x)) {
        st.a = 1;
        st.b = &D_0026C638;
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
