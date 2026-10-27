// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.78% NEARMISS).
// Fixes read from the original instructions:
//  - The CD state word D_00241D48 and the semaphore D_00241D0C are both volatile
//    globals (shared with the RPC side). The volatile store/load
//    pair keeps the success arm's state clear ahead of the semaphore's address setup,
//    which is why the original rebuilds the semaphore's upper address half there
//    instead of reusing the one from the branch delay slot (the "%hi-CSE wall").
//  - The success arm's semaphore read sits in SignalSema's call delay slot: that is
//    the 2.96 assembler filling the call slot with the preceding load, not cc1.
//    The earlier per-arm `*(volatile int *)&` cast is no longer needed.
//
// One RPC request (function 0x16) on the SIF RPC client D_0027AF60 with a 4-byte reply:
// wait for the channel (func_00112E28 with command 0x1E, returns 0 if busy), set the
// state word D_00241D48 to 8, issue the call into the reply buffer D_0027AB40. On failure,
// release the semaphore, clear the state and return 0; on success clear the state,
// release the semaphore and return the reply word read through the uncached mirror
// (address | 0x20000000).

extern volatile int D_00241D48;
extern volatile int D_00241D0C;
extern int D_0027AB40;
extern int D_0027AF60;

extern int func_00112E28(int);
extern int func_0010E8A8(void *a, int b, int c, int d, int e, void *f, int g, int h, int i);
extern int SignalSema(int);

int func_00113478(void) {
    int *p;
    int r;
    if (func_00112E28(0x1E) == 0) {
        return 0;
    }
    p = &D_0027AB40;
    D_00241D48 = 8;
    r = func_0010E8A8(&D_0027AF60, 0x16, 0, 0, 0, p, 4, 0, 0);
    if (r < 0) {
        SignalSema(D_00241D0C);
        D_00241D48 = 0;
        return 0;
    }
    D_00241D48 = 0;
    SignalSema(D_00241D0C);
    return *(int *)((unsigned int)p | 0x20000000);
}
