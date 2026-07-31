// COMPILER: eegcc
// CFLAGS: -O2
//
// SDK/lowmem (ee-gcc unit). One-shot dispatcher gated on the D_0024141C flag:
// when the flag is set it takes the D_0026B668 path (func_0010A3A8), otherwise
// it dispatches on the mode selector D_002414AC -- mode 3 goes to func_00107E88
// with the D_00241304 handle, every other mode to func_00107FA0 with the
// D_00241310 / D_0024131C pair. The caller's 1-based index is passed on as
// index - 1. Every path clears D_0024141C on the way out.
//
// Matching notes (ee-gcc 2.9-991111-01 -O2, objdiff 100.0):
//  * the `D_0024141C = 0` clear is written out per-branch, not once after the
//    if/else: ee-gcc places each store in its own branch's delay slot, which a
//    single trailing store at the join point would not reproduce. The %hi is
//    hoisted into callee-saved $s0 at entry because it is used four times.

extern int D_0024141C;
extern int D_002414AC;
extern int D_00241304;
extern int D_00241310;
extern int D_0024131C;
extern unsigned char D_0026B668[];

extern void func_0010A3A8();
extern void func_00107E88();
extern void func_00107FA0();

void sub_the_second_field_is_missing(int index)
{
    if (D_0024141C != 0) {
        func_0010A3A8(D_0026B668);
        D_0024141C = 0;
    } else if (D_002414AC == 3) {
        func_00107E88(D_00241304, index - 1);
        D_0024141C = 0;
    } else {
        func_00107FA0(D_00241310, D_0024131C, index - 1);
        D_0024141C = 0;
    }
}
