// COMPILER: eegcc
// CFLAGS: -O2

// SDK pad/device query. Resolves the per-port control block with
// func_0010EFA8(a0); fails with -1 if the subsystem flag D_00241C88 is clear
// and with -9 if the block is missing or its state word (+4) is zero.
// For mode 1 it reports through *a2 whether the secondary device is present
// (func_0010EA60(&D_00279210) -> a1 when nonzero, 0 otherwise) and returns 0;
// otherwise it returns 0 when the state word has bit 0x8000 set, and the
// cached status word D_00279040 when it does not.
//
// `d` is the 16-byte stack local the target's frame reserves: the prologue
// allocates 0x40 and parks s0/s1/ra at 0x10/0x20/0x30, leaving 16 unused bytes
// at the bottom of the frame. ee-gcc 2.9 keeps an unreferenced local array's
// slot, which is what reproduces that frame size (without it the frame is 0x30
// and every save offset shifts down by 16).
extern void *func_0010EFA8(int);
extern int func_0010EA60(void *);

extern int D_00241C88;
extern char D_00279210;
extern int D_00279040;

int func_0010F7D8(int a0, int a1, int *a2) {
    int d[4];
    int *p;
    int flags;

    p = (int *)func_0010EFA8(a0);
    if (D_00241C88 == 0)
        return -1;
    if (p == 0)
        return -9;
    flags = p[1];
    if (flags == 0)
        return -9;
    if (a1 == 1) {
        if (func_0010EA60(&D_00279210) == 0)
            *a2 = 0;
        else
            *a2 = a1;
        return 0;
    }
    if ((flags & 0x8000) != 0)
        return 0;
    return D_00279040;
}
