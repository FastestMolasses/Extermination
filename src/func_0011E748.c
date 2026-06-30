// NEARMISS func_0011E748  (vram 0x0011E748, 0x114 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.58% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler wall. Body byte-identical and float-register assignment correct (f20=x arg saved first, f21=r=func_0011CB90() result; returns r on ALL paths, func_00127758's return discarded). Only TWO residual differences, both pure ee-gcc 2.9 list-scheduler artifacts: (1) prologue adjacent-op swap - expected ...
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
