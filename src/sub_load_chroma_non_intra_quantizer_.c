// COMPILER: eegcc
// CFLAGS: -O2
//
// SDK/lowmem (ee-gcc unit). Allocates four handles of kind 1 from func_00106948
// and, for each one that succeeds, runs its registration step. The first two
// handles are published into the globals D_00241BA0 / D_00241BA4 and each is
// followed by the func_001063E8 / func_001063B8(base) / func_001063E8 sequence
// with the DMA/segment bases 0x50000000 and 0x58000000. The last two handles
// are not stored -- they only gate a func_0010A3A8 call on the D_0026B4B0 and
// D_0026B4D8 descriptors, the second of which is a tail call.
//
// Matching notes (ee-gcc 2.9-991111-01 -O2, objdiff 100.0):
//  * eegcc idiom-29 is load-bearing: `p = &D_00241BA0; *p = t1 = func(1);`
//    forces the store's `lui %hi` BEFORE the call, into the callee-saved $s0
//    that survives it (and lets the lui fill the `jal` delay slot). Writing
//    `D_00241BA0 = func_00106948(1);` directly puts the %hi after the call in a
//    caller-saved register, drops the $s0 save/restore and the frame from 0x20
//    to 0x10, and costs ~17%.
//  * a single reused `p` is fine here (only one callee-saved reg is live at a
//    time), but the call results need their own temps so the `beqz` tests $v0
//    directly instead of reloading the global.

extern int D_00241BA0;
extern int D_00241BA4;
extern unsigned char D_0026B4B0[];
extern unsigned char D_0026B4D8[];

extern int func_00106948();
extern void func_001063B8();
extern void func_001063E8();
extern void func_0010A3A8();

void sub_load_chroma_non_intra_quantizer_(void)
{
    int *p;
    int t1, t2;

    p = &D_00241BA0;
    *p = t1 = func_00106948(1);
    if (t1 != 0) {
        func_001063E8();
        func_001063B8(0x50000000);
        func_001063E8();
    }

    p = &D_00241BA4;
    *p = t2 = func_00106948(1);
    if (t2 != 0) {
        func_001063E8();
        func_001063B8(0x58000000);
        func_001063E8();
    }

    if (func_00106948(1) != 0) {
        func_0010A3A8(D_0026B4B0);
    }

    if (func_00106948(1) != 0) {
        func_0010A3A8(D_0026B4D8);
    }
}
