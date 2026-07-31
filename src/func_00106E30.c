// COMPILER: eegcc
// CFLAGS: -O2
//
// Low-level subsystem init (SDK/lowmem region, ee-gcc unit). Allocates a batch of
// handles/IDs from the allocator func_00106948(kind) and publishes each one into its
// own global. First four go into the 4-entry table at D_00241498; three of the later
// IDs are additionally packed into bitfields of the hardware control word at
// 0x10002010 (2-bit field at bit 16 from the D_002414A8 id, and single bits 22/21/20
// from the D_00241BB4 / D_002414BC / D_00241BB8 ids). The D_002414AC id is also
// latched into field +0xD4 of the object reached through D_002412F4[+0x40], but only
// if that field is still zero. The final block (D_002414D0..D_002414E0, kinds
// 1/3/1/7/8) is only allocated when the D_002414CC allocation succeeded (non-zero).
//
// Matching notes (ee-gcc 2.9-991111-01 -O2, objdiff 100.0):
//  * the local `int *p` reassigned before each store is load-bearing: it makes ee-gcc
//    emit the store address (`lui %hi`) BEFORE the call, so the %hi lands in a
//    callee-saved register that survives the call -- exactly what the target does.
//    Writing `D_xxx = func_00106948(1);` directly emits the %hi after the call in a
//    caller-saved register and loses ~35%.
//  * one temp per call result (t1..t6, not a single reused `t`) is what removes the
//    spurious `daddu aN, v0, zero` copies: a single reused local becomes one pseudo
//    with a function-long live range and cannot coalesce with the $v0 return register.
//  * `cur` (reading obj+0xD4 before the D_002414AC store) is what lets the delay-slot
//    pass fill the `bnez` with that store instead of emitting an annulled `beqzl`.
//  * the 0x10002010 accesses must NOT be volatile: volatile blocks the delay-slot fill
//    of the final `sw` and costs the last 2%.

extern char *D_002412F4;
extern int func_00106948(int);
extern int D_00241498[4];
extern int D_002414A8;
extern int D_002414AC;
extern int D_002414B0;
extern int D_002414B4;
extern int D_002414B8;
extern int D_002414BC;
extern int D_002414C0;
extern int D_002414C4;
extern int D_002414C8;
extern int D_002414CC;
extern int D_002414D0;
extern int D_002414D4;
extern int D_002414D8;
extern int D_002414DC;
extern int D_002414E0;
extern int D_00241BB4;
extern int D_00241BB8;

#define REG (*(unsigned int *)0x10002010)

void func_00106E30(void) {
    char *obj = *(char **)(D_002412F4 + 0x40);
    int *pa = &D_002414A8;
    int *pb = &D_002414AC;
    int *p;
    int t1, t2, t3, t4, t5, t6;
    int cur;

    D_00241498[0] = func_00106948(4);
    D_00241498[1] = func_00106948(4);
    D_00241498[2] = func_00106948(4);
    D_00241498[3] = func_00106948(4);

    *pa = t1 = func_00106948(2);
    REG = (REG & 0xFFFCFFFF) | (t1 << 16);

    t2 = func_00106948(2);
    cur = *(int *)(obj + 0xD4);
    *pb = t2;
    if (cur == 0) {
        *(int *)(obj + 0xD4) = t2;
    }

    p = &D_002414B0; *p = func_00106948(1);
    p = &D_002414B4; *p = func_00106948(1);
    p = &D_002414B8; *p = func_00106948(1);

    p = &D_00241BB4; *p = t3 = func_00106948(1);
    REG = (REG & 0xFFBFFFFF) | (t3 << 22);

    p = &D_002414BC; *p = t4 = func_00106948(1);
    REG = (REG & 0xFFDFFFFF) | (t4 << 21);

    p = &D_00241BB8; *p = t5 = func_00106948(1);
    REG = (REG & 0xFFEFFFFF) | (t5 << 20);

    p = &D_002414C0; *p = func_00106948(1);
    p = &D_002414C4; *p = func_00106948(1);
    p = &D_002414C8; *p = func_00106948(1);

    p = &D_002414CC; *p = t6 = func_00106948(1);
    if (t6 != 0) {
        p = &D_002414D0; *p = func_00106948(1);
        p = &D_002414D4; *p = func_00106948(3);
        p = &D_002414D8; *p = func_00106948(1);
        p = &D_002414DC; *p = func_00106948(7);
        p = &D_002414E0; *p = func_00106948(8);
    }
}
