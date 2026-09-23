// NEARMISS func_0011E748  (vram 0x0011E748, 0x114 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.58% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler wall. Float-register assignment: x is kept in f20 and r = func_0011CB90() in f21.
// RETURN VALUE (corrected against the .s): r is returned on the early paths (D_0026C5D0 == -1,
// func_0011E080(x) != 0, or !(x < 0)), but NOT on the error path. There the original calls
// func_00127758 with the exception record's +0x18 double (0, or D_0026C650 when mode != 0, as
// possibly rewritten by the matherr in func_0011DB90) and goes straight to the epilogue without
// copying r back into f0, so the function returns func_00127758's float result (e.g. 0x7FB00000 for
// sqrtf(negative normal); see the port's docs/SDK_MATH_ORIGINAL.md). The C below declares
// func_00127758 void and ends in `return r`, so on that path it is NOT body-correct; being NEARMISS
// it is never linked (the boot ELF is unaffected). An earlier version of this header said "returns
// r on ALL paths, func_00127758's return discarded" and "body byte-identical" with only two
// list-scheduler residuals; the error-path return is a real behavioural difference, so those
// statements were wrong.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_0026C5D0;
extern long D_0026C648;
extern long D_0026C650;
extern float func_0011CB90(void);
extern int func_0011E080(float);
extern long func_00128350(float);
extern int func_0011DB90(void *);
extern int *func_0011FD78(void);
extern void func_00127758(long);

float func_0011E748(float x)
{
    int mode;
    float r;
    long h;
    char buf[0x24];

    r = func_0011CB90();
    mode = D_0026C5D0;
    if (mode == -1) {
        return r;
    }
    if (func_0011E080(x) != 0) {
        return r;
    }
    if (!(x < 0.0f)) {
        return r;
    }

    *(int *)(buf + 0x0) = 1;
    *(int *)(buf + 0x4) = (int)&D_0026C648;
    *(int *)(buf + 0x20) = 0;
    h = func_00128350(x);
    *(long *)(buf + 0x8) = h;
    *(long *)(buf + 0x10) = h;
    if (mode == 0) {
        *(long *)(buf + 0x18) = 0;
    } else {
        *(long *)(buf + 0x18) = D_0026C650;
    }

    if (D_0026C5D0 == 2 || func_0011DB90(buf) == 0) {
        *func_0011FD78() = 0x21;
    }
    if (*(int *)(buf + 0x20) != 0) {
        *func_0011FD78() = *(int *)(buf + 0x20);
    }
    func_00127758(*(long *)(buf + 0x18));
    return r;
}
