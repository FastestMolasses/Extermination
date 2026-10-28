// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 70.38% NEARMISS).
// Fixes read from the original instructions:
//  - the function returns void: the final DeleteSema is a void-statement tail
//    call (`return DeleteSema(...)` makes ee-gcc emit a call plus a separate return);
//  - the subsystem state words are volatile (shared with the service thread),
//    as in the byte-matched sibling func_001118B8;
//  - assembled with the 2.96 gas, which fills the call delay slots like the
//    original.

// Subsystem shutdown: if the service thread exists (id in D_00241D04), mark the
// request record D_00241D48 with -1 and wake the thread via SignalSema on the
// third semaphore; then delete all three semaphores (D_00241D08, D_00241D0C,
// D_00241D10). SignalSema / DeleteSema take the semaphore id by value.

extern volatile int D_00241D04;
extern volatile int D_00241D08;
extern volatile int D_00241D0C;
extern volatile int D_00241D10;
extern volatile int D_00241D48;
extern int SignalSema(int sema_id);
extern int DeleteSema(int sema_id);

void func_00111950(void) {
    if (D_00241D04 != 0) {
        D_00241D48 = -1;
        SignalSema(D_00241D10);
    }
    DeleteSema(D_00241D08);
    DeleteSema(D_00241D0C);
    DeleteSema(D_00241D10);
}
