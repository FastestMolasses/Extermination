// COMPILER: eegcc
// CFLAGS: -O2
//
// SDK/MPEG init: allocates a set of resources via func_00106948(kind) and stashes the
// handles in the D_002414xx table, then runs the three-stage startup chain. The extra
// handles are only created for the 2/3 modes (D_0024147C).
//
// ee-gcc notes: the trailing `j func_00107098` is a VOID statement tail-call — writing
// `return func_00107098();` makes gcc emit `jal; nop; ...; jr ra` instead. The dedicated
// pointer locals (idiom-29) are what force each store's `lui %hi` into a callee-saved
// register BEFORE the corresponding `jal`.

extern int func_00106948(int);
extern void func_00107060(void);
extern void func_00106D80(void);
extern void func_00107098(void);

extern int D_00241478;
extern int D_0024147C;
extern int D_00241480;
extern int D_00241484;
extern int D_00241488;
extern int D_0024148C;
extern int D_00241490;

void func_00106CB0(void)
{
    int *pmode = &D_0024147C;
    int *p;

    p = &D_00241478;
    *p = func_00106948(10);
    *pmode = func_00106948(3);
    p = &D_00241480;
    *p = func_00106948(0x10);
    if (*pmode == 2 || *pmode == 3) {
        p = &D_00241484;
        *p = func_00106948(1);
        p = &D_00241488;
        *p = func_00106948(3);
    }
    if (*pmode == 3) {
        p = &D_0024148C;
        *p = func_00106948(1);
        p = &D_00241490;
        *p = func_00106948(3);
    }
    func_00107060();
    func_00106D80();
    func_00107098();
}
