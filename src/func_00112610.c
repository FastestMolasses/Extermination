// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 87.93% NEARMISS).
// Fixes read from the original instructions:
//  - func_0010DFD8 takes (buffer, size): it is called with the request buffer and 0x18
//    (the old C declared it (void)).
//  - The RPC state words D_00241D48 / D_00241D14 / D_00241D08 are volatile, as in the
//    byte-matched RPC siblings: their stores and reload keep source order.
//  - Assembled with the 2.96 gas, which fills the SignalSema call delay slots.
//  - The request buffer is a struct whose two buffer-address fields are ints
//    (a pointer type for them reorders the first store).
//  - The debug print is the int-returning variadic SDK printf.
//
// Sends RPC request 0xD (mode 1) through the client at D_00279E00: unless the RPC layer is
// in state 6 or the binding check for slot 5 fails, it fills the 0x18-byte request
// (three ints, three bytes copied from a3, and the addresses of two receive buffers),
// writes it back from the data cache, marks the call pending and issues it with
// func_001115D0 as the completion callback. On failure it clears the pending flags,
// signals the semaphore and returns 0; on success it optionally prints a debug message,
// signals the semaphore and returns 1.

struct Req {
    int a;
    int b;
    int c;
    unsigned char d[4];
    int buf0;
    int buf1;
};

extern struct Req D_00279F40;
extern int D_00279E80;
extern int D_0027A2C0;
extern volatile int D_00241D48;
extern volatile int D_00241D14;
extern int D_00279E00;
extern int D_00241CF8;
extern volatile int D_00241D08;
extern char D_0026BC78[];

extern int func_00112088(void);
extern int func_00111F18(int a0);
extern void func_0010DFD8(void *p, int size);
extern int func_0010E8A8(void *cd, int fno, int mode, void *send, int ssize, void *recv, int rsize, void (*end)(void), void *arg);
extern int SignalSema(int sema);
extern int func_00122B58(const char *fmt, ...);
extern void func_001115D0(void);

int func_00112610(int a0, int a1, int a2, unsigned char *a3) {
    if (func_00112088() == 6) {
        return 0;
    }
    if (func_00111F18(5) == 0) {
        return 0;
    }
    D_00279F40.a = a0;
    D_00279F40.b = a1;
    D_00279F40.c = a2;
    D_00279F40.d[0] = a3[0];
    D_00279F40.d[1] = a3[1];
    D_00279F40.d[2] = a3[2];
    D_00279F40.buf0 = (int)&D_00279E80;
    D_00279F40.buf1 = (int)&D_0027A2C0;
    func_0010DFD8(&D_00279F40, 0x18);
    D_00241D48 = 1;
    D_00241D14 = 1;
    if (func_0010E8A8(&D_00279E00, 0xD, 1, &D_00279F40, 0x18, 0, 0, func_001115D0, &D_00241D48) < 0) {
        D_00241D48 = 0;
        D_00241D14 = 0;
        SignalSema(D_00241D08);
        return 0;
    }
    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BC78);
    }
    SignalSema(D_00241D08);
    return 1;
}
