// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Zero-init of a 12-byte record living at a large fixed offset (+0x50000)
// from the base pointer a0: clears words at +0x50000 (a0[0x14000]) and
// +0x50004 (a0[0x14001]) to 0, then writes the constant 0x50000 to +0x50008
// (a0[0x14002]) -- likely a self-referential base/size or capacity field.
// The large offset forces CW's `lui at,0x5; addu at,a0,at; sw ...,disp(at)`
// addressing macro (assembler temp $at, recomputed per store, low part kept
// as the store displacement).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the lone
// residual under 991202 was the list-scheduler hoisting the value-load
// `lui v1,0x5` ahead of the final address computation (81.8%); the 2.3.3
// scheduler emits it after the addu, exactly as CW does. Verified objdiff
// 100% byte-identical vs build/expected/func_002039A0.o.
void func_002039A0(int *a0) {
    a0[0x14001] = 0;
    a0[0x14000] = 0;
    a0[0x14002] = 0x50000;
}
