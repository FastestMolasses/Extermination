// COMPILER: eegcc
// CFLAGS: -O2
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 87.08% NEARMISS). Fixes read from
// the original instructions:
// - The routine is newlib's dtoa helper _pow5mult (b * 5^k): rewritten in newlib's own shape
//   (early `return b` when k>>2 is zero, `if (!(p5 = ptr->_p5s))` lazy init, for(;;) loop with
//   the in-test assignments). The old goto-into-loop body gave ee-gcc a different dataflow
//   (5 saved registers and a larger frame).
// - The Bfree callee (func_00121BA0) returns void, matching its definition in src/.
//
// Multiply Bigint b by 5^k. The low two bits of k select 5/25/125 from D_0026C808 via
// multadd (func_00121BD0); the remaining bits walk the cached chain of 5^(4*2^n) powers
// anchored at ptr->_p5s (+0x48), created on demand with i2b(625) (func_00121F78) and
// mult(p5, p5) (func_00121FB0); each used power multiplies b and the old b is freed.
typedef struct _Bigint { struct _Bigint *_next; int _k; } _Bigint;
struct _reent { int pad[18]; _Bigint *_p5s; };
extern void func_00121BA0(struct _reent *, _Bigint *);
extern _Bigint *func_00121BD0(struct _reent *, _Bigint *, int, int);
extern _Bigint *func_00121F78(struct _reent *, int);
extern _Bigint *func_00121FB0(struct _reent *, _Bigint *, _Bigint *);
extern int D_0026C808[];

_Bigint *func_001221E0(struct _reent *ptr, _Bigint *b, int k) {
    _Bigint *b1, *p5, *p51;
    int i;

    if ((i = k & 3))
        b = func_00121BD0(ptr, b, D_0026C808[i - 1], 0);

    if (!(k >>= 2))
        return b;
    if (!(p5 = ptr->_p5s)) {
        p5 = ptr->_p5s = func_00121F78(ptr, 625);
        p5->_next = 0;
    }
    for (;;) {
        if (k & 1) {
            b1 = func_00121FB0(ptr, b, p5);
            func_00121BA0(ptr, b);
            b = b1;
        }
        if (!(k >>= 1))
            break;
        if (!(p51 = p5->_next)) {
            p51 = p5->_next = func_00121FB0(ptr, p5, p5);
            p51->_next = 0;
        }
        p5 = p51;
    }
    return b;
}
