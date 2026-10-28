// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 74.52% NEARMISS).
// Fixes read from the original instructions:
//  - The RPC state words are volatile, as in the byte-matched RPC siblings
//    (func_00112610, func_001129E8): the original re-reads D_00241D48 after storing it
//    and again for the 0xB test, and reads every flag word fresh. Not a register-
//    allocation wall.
//
// RPC completion callback (func_001129E8 passes it to func_0010E8A8 with &D_00241D48 as
// its argument): stores the received status word *p in D_00241D48 and copies it to
// D_00241D4C. When the D_00241D18 flag is set and the status is 0xB, it just clears
// D_00241D48 and the pending flag D_00241D14 and returns. Otherwise it signals the
// semaphore D_00241D08 from interrupt context; if both D_00241D04 and D_00241D44 are
// set it also signals D_00241D10, else it clears D_00241D14; finally it clears
// D_00241D48.

extern volatile int D_00241D04;
extern volatile int D_00241D08;
extern volatile int D_00241D10;
extern volatile int D_00241D14;
extern volatile int D_00241D18;
extern volatile int D_00241D44;
extern volatile int D_00241D48;
extern volatile int D_00241D4C;

extern int iSignalSema(int sema);

void func_001115D0(int *p) {
    D_00241D48 = *p;
    D_00241D4C = D_00241D48;
    if (D_00241D18 != 0) {
        if (D_00241D48 == 0xB) {
            D_00241D48 = 0;
            D_00241D14 = 0;
            return;
        }
    }
    iSignalSema(D_00241D08);
    if (D_00241D04 != 0 && D_00241D44 != 0) {
        iSignalSema(D_00241D10);
    } else {
        D_00241D14 = 0;
    }
    D_00241D48 = 0;
}
