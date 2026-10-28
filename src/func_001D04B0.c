// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an 81.67% NEARMISS).
// Fixes read from the original instructions:
//  - The argument-copy schedule (b copied into a saved FPU register before the
//    first call, a2 saved in that call's delay slot, obj moved into a1 in the
//    second call's delay slot) follows the source order of the parameters, not
//    the registers they arrive in. Under the PS2 ABI floats travel in f12/f13
//    and integers in a0..a2 whatever their position in the list, so the
//    parameter order is invisible to callers but steers mwcc's copy order:
//      * this function's floats are declared right after obj
//        (obj, a, b, a1, a2; same registers as (obj, a1, a2, a, b));
//      * func_001CFA60 is declared with its two floats first
//        (a, b, block, obj; same registers as its src/ definition, which
//        also byte-matches when its own floats are declared first).
//
// Registers a drawn sprite for an object: the display handle comes from
// func_001CCF70(obj + 0x30); func_001CFA60 builds the 0x60-byte draw block
// from the object with the two float parameters, and func_001CFBE0(handle,
// a1, a2, block, 0) queues it.
extern int func_001CCF70(void *a0);
extern void func_001CFA60(float a, float b, void *block, void *obj);
extern void func_001CFBE0(int handle, int a1, void *a2, void *block, int a4);

void func_001D04B0(char *obj, float a, float b, int a1, void *a2) {
    char block[0x60];
    int handle = func_001CCF70(obj + 0x30);

    func_001CFA60(a, b, block, obj);
    func_001CFBE0(handle, a1, a2, block, 0);
}
