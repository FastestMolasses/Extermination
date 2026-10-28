// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 79.49% NEARMISS).
// Fixes read from the original instructions:
//  - The RPC call func_0010E8A8 takes nine arguments (client, function 5, mode 1, send
//    buffer, send size 4, no receive buffer, receive size 0, completion callback
//    func_001115D0, callback argument &D_00241D48); the old C passed eight in the wrong
//    order, which is what looked like a "branch-likely wall".
//  - func_0010DFD8 takes (buffer, size).
//  - The RPC state words D_00241D48 / D_00241D14 / D_00241D08 are volatile, as in the
//    byte-matched RPC siblings (func_00112610).
//  - Assembled with the 2.96 gas, which fills the failure path's SignalSema delay slot.
//
// Sends the one-word RPC request 5 (mode 1) through the client at D_00279E00: unless the
// RPC layer is in state 6 or the binding check for slot 9 fails, it stores a0 in the
// request word D_0027AB10, writes it back from the data cache, marks the call pending
// (D_00241D48 = 4, D_00241D14 = 1) and issues it with func_001115D0 as the completion
// callback. On failure it clears the pending flags, signals the semaphore and returns 0;
// on success it signals the semaphore and returns 1.

extern int D_0027AB10;
extern volatile int D_00241D48;
extern volatile int D_00241D14;
extern int D_00279E00;
extern volatile int D_00241D08;

extern int func_00112088(void);
extern int func_00111F18(int a0);
extern void func_0010DFD8(void *p, int size);
extern int func_0010E8A8(void *cd, int fno, int mode, void *send, int ssize, void *recv, int rsize, void (*end)(int *), void *arg);
extern int SignalSema(int sema);
extern void func_001115D0(int *p);

int func_001129E8(int a0) {
    if (func_00112088() == 6) {
        return 0;
    }
    if (func_00111F18(9) == 0) {
        return 0;
    }
    D_0027AB10 = a0;
    func_0010DFD8(&D_0027AB10, 4);
    D_00241D48 = 4;
    D_00241D14 = 1;
    if (func_0010E8A8(&D_00279E00, 5, 1, &D_0027AB10, 4, 0, 0, func_001115D0, (void *)&D_00241D48) < 0) {
        D_00241D48 = 0;
        D_00241D14 = 0;
        SignalSema(D_00241D08);
        return 0;
    }
    SignalSema(D_00241D08);
    return 1;
}
