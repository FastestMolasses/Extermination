// NEARMISS func_0010D160  (vram 0x0010D160, 0x168 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.94% via ee-gcc 2.9-991111-01 (-O2 -mno-split-addresses). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc addressing + list-scheduler residual (4 bytes over: 364 vs 360). Frame size 0x40, callee-save set s0/s1/s2, both loop shapes, branch polarity and BOTH sibling tail-calls (j func_0010D890) now match exactly. Three residuals, none source-fixable: (1) the 3 literal-pool double loads are the ga...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2 -mno-split-addresses

// SEMANTICS: printf's %e/%f back end — prints a double in scientific notation.
// The double is carried in a single 64-bit GPR and every arithmetic step goes
// through the SDK's soft-float helpers, so they are called explicitly here:
//   func_001274B0(a,b) compare (<0 / 0 / >0)   func_00126F20(a,b) subtract
//   func_00126F88(a,b) multiply                func_00127230(a,b) divide
//   func_00125758(a)   double -> int           func_0010D008(n) int -> string
//   func_0010D890(fmt, v) formatted print (tail-called)
// D_0026B868/70/78 are the literal-pool doubles; 10.0 (0x8048 << 47) and
// 1.0 (0xFFC0 << 46) are cheap enough that the compiler materialises them inline.
// The value is made positive (emitting '-' through the D_00241C74 putchar hook),
// normalised into [1,10) while counting the decimal exponent, scaled by
// D_0026B878, printed as an integer mantissa, then the exponent is printed with
// D_0026B858 (>= 0) or D_0026B860 (< 0).

extern double D_0026B868;
extern double D_0026B870;
extern double D_0026B878;
extern char D_0026B850[];
extern char D_0026B858[];
extern char D_0026B860[];
extern void (*D_00241C74)(int c);

extern int func_001274B0(double a, double b);
extern double func_00126F20(double a, double b);
extern double func_00126F88(double a, double b);
extern double func_00127230(double a, double b);
extern int func_00125758(double a);
extern char *func_0010D008(int v);
extern void func_0010D890(char *fmt, int v);

void func_0010D160(double x)
{
    int exp;
    char *fmt;

    fmt = D_0026B850;
    exp = 0;
    if (func_001274B0(x, 0.0) < 0) {
        x = func_00126F20(0.0, x);
        (*D_00241C74)(0x2D);
    }
    if (func_001274B0(x, D_0026B868) < 0) {
        while (func_001274B0(x, D_0026B870) < 0) {
            exp--;
            x = func_00126F88(x, 10.0);
        }
    } else {
        if (func_001274B0(x, 1.0) >= 0) {
            while (func_001274B0(x, 1.0) >= 0) {
                exp++;
                x = func_00127230(x, 10.0);
            }
        }
    }
    func_0010D890(fmt, (int)func_0010D008(func_00125758(func_00126F88(x, D_0026B878))));
    if (exp >= 0) {
        func_0010D890(D_0026B858, exp);
    } else {
        func_0010D890(D_0026B860, exp);
    }
}
