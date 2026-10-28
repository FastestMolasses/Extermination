// NEARMISS func_0010D160  (vram 0x0010D160, 0x168 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.67% via ee-gcc 2.9-991111-01 (-O2). The instruction stream is byte-identical
// (same .text size 0x168); the only remaining difference is relocation spelling: the three
// double constants are this object's own .rodata (0.1, 0.1, 1000000.0, 24 bytes, 8-aligned,
// identical to the original bytes at 0x0026B868..0x0026B87F), so the loads relocate against
// .rodata+0/8/16 instead of the splat names D_0026B868/70/78. Pinning this object's .rodata
// at 0x0026B868 (tools/decomp/rodata_pin.py) should make it link-identical.
// Remaining differences: local .rodata relocation spelling only (needs a rodata pin).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// Fixes read from the original instructions (wave 2026-10-27):
//  - The doubles were literal constants in the source (constant-pool loads through the
//    assembler temporary), not extern globals; the string addresses are split (lui hoisted
//    into a saved register), so the file builds without -mno-split-addresses.
//  - No format-pointer local at the top: the "0.%d" address is formed where it is used,
//    and the mantissa string is computed into a local before that call.

// SEMANTICS: printf's %e/%f back end — prints a double in scientific notation.
// The double is carried in a single 64-bit GPR and every arithmetic step goes
// through the SDK's soft-float helpers, so they are called explicitly here:
//   func_001274B0(a,b) compare (<0 / 0 / >0)   func_00126F20(a,b) subtract
//   func_00126F88(a,b) multiply                func_00127230(a,b) divide
//   func_00125758(a)   double -> unsigned 64-bit (__fixunsdfdi)
//   func_0010D008(m)   64-bit double bit pattern -> int (saturates at 9999)
//   func_0010D890(fmt, v) formatted print (tail-called)
// 0.1 (twice) and 1000000.0 live in the constant pool (0x0026B868/70/78); 10.0 and
// 1.0 are cheap enough that the compiler materialises them inline.
// The value is made positive (emitting '-' through the D_00241C74 putchar hook),
// normalised into [0.1,1) while counting the decimal exponent, scaled by 1000000.0,
// converted to an integer mantissa and printed after "0." (D_0026B850), then the
// exponent is printed with "e+%d" (D_0026B858, >= 0) or "e%d" (D_0026B860, < 0).

extern char D_0026B850[];
extern char D_0026B858[];
extern char D_0026B860[];
extern void (*D_00241C74)(int c);

extern int func_001274B0(double a, double b);
extern double func_00126F20(double a, double b);
extern double func_00126F88(double a, double b);
extern double func_00127230(double a, double b);
extern unsigned long func_00125758(double a);
extern int func_0010D008(unsigned long m);
extern void func_0010D890(char *fmt, int v);

void func_0010D160(double x)
{
    int exp;
    int m;

    exp = 0;
    if (func_001274B0(x, 0.0) < 0) {
        x = func_00126F20(0.0, x);
        (*D_00241C74)(0x2D);
    }
    if (func_001274B0(x, 0.1) < 0) {
        while (func_001274B0(x, 0.1) < 0) {
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
    m = func_0010D008(func_00125758(func_00126F88(x, 1000000.0)));
    func_0010D890(D_0026B850, m);
    if (exp >= 0) {
        func_0010D890(D_0026B858, exp);
    } else {
        func_0010D890(D_0026B860, exp);
    }
}
