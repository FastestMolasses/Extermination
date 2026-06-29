// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// One-shot interrupt-restore/init: if the gate D_00275C84 is set, call
// func_00206010(D_007A55A0) and clear the gate, then sync + ei and return 0.
// The 128-bit ra save/restore (sq/lq) and the `ei` come from the asm block
// plus the standard mwcc EE frame. Returning int 0 reproduces the
// `paddub v0,zero,zero` zero-return idiom and places the gate load in v0.
//
// 991202 leaves a delay-slot nop residual; mwcc 2.3.3 byte-matches.
extern int D_00275C84;
extern char D_007A55A0[8];
extern void func_00206010(char *);

int func_00206170(void) {
    if (D_00275C84) {
        func_00206010(D_007A55A0);
        D_00275C84 = 0;
    }
    asm { sync; ei; }
    return 0;
}
