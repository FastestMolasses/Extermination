// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.78% NEARMISS). Fixes read from the original instructions:
// - The RPC lock semaphore id D_00241D08 and the busy flag D_00241D48 are volatile globals
//   (as in the byte-matched siblings func_00112440 / func_00112AD0): the busy-flag clear in
//   the RPC-failure block now stays ahead of the semaphore-id address setup, as in the original.
// - SignalSema is declared returning void, as in the siblings (the result is never used).

//
// SIF-RPC wrapper (same family as the already-committed func_00113F68): check the RPC client is
// bound, publish the send buffer pointer, write back the D-cache for both buffers, raise the
// busy flag D_00241D48 to 3, do the blocking RPC (function #4), then copy the reply out of the
// UNCACHED (| 0x20000000) mirror of the receive buffer into the caller's buffer.  The reply
// header word at D_00279B84 selects the long (0x810) or short (0x400) payload; the word at
// D_00279B80 is the value returned to the caller.  SignalSema() (0x0010B840, EE syscall 66)
// releases the RPC lock.
//

extern int func_00111F18(int);
extern void func_0010DFD8(void *, int);
extern int func_0010E8A8(void *, int, int, void *, int, void *, int, void (*)(void *), void *);
extern void SignalSema(int);
extern void *memcpy(void *, const void *, unsigned int);

extern volatile int D_00241D08;
extern volatile int D_00241D48;
extern unsigned char D_00279B80[];
extern unsigned char D_00279E00[];
extern void *D_0027A2D0;
extern unsigned char D_0027A300[];

int func_00112758(void *dst)
{
    int ret;

    if (func_00111F18(8) == 0)
        return 0;

    D_0027A2D0 = D_0027A300;
    func_0010DFD8(D_0027A300, 0x810);
    func_0010DFD8(&D_0027A2D0, 0xC);

    D_00241D48 = 3;
    if (func_0010E8A8(D_00279E00, 4, 0, &D_0027A2D0, 0xC, D_00279B80, 8, 0, 0) < 0) {
        D_00241D48 = 0;
        SignalSema(D_00241D08);
        return 0;
    }
    D_00241D48 = 0;

    ret = *(int *)(D_00279B80 + 0);
    if (*(int *)(D_00279B80 + 4) == 0)
        memcpy(dst, (void *)((unsigned int)D_0027A300 | 0x20000000), 0x400);
    else
        memcpy(dst, (void *)((unsigned int)D_0027A300 | 0x20000000), 0x810);

    SignalSema(D_00241D08);
    return ret;
}
