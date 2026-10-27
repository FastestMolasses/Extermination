// COMPILER: eegcc
// CFLAGS: -O2
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 92.36% NEARMISS).
// Fixes read from the original instructions:
// - The request block at D_0027B170 is a 0x414-byte struct (five words, then a
//   0x400-byte name buffer), not raw int-array indexing. The struct field
//   accesses change ee-gcc's alias/scheduling so the buffer-pointer store lands
//   in the copy call's delay slot, as in the original (the "list-scheduler wall"
//   was our int-array spelling).
// - The terminator is written at name[0x3FF] (offset 0x413); the send size is
//   sizeof the struct (0x414).
//
// SDK RPC wrapper (ee-gcc 2.9): fails with the latched error if one is set, or
// -0x64 if the RPC client is not bound; otherwise fills the request (two args,
// the D_0027B680 buffer pointer, a bounded copy of the name string), passes
// that 0x400-byte buffer to func_0010DFD8, and issues RPC function 0xC with callback
// func_00114BA8 (callback argument a3). A failed call latches error 0xC.
extern int D_00241D68;
struct s_b0c0 { char pad[0x24]; int f24; };
extern struct s_b0c0 D_0027B0C0;
struct req { int a; int b; int c; int d; char *buf; char name[0x400]; };
extern struct req D_0027B170;
extern char D_0027B680[];
extern int D_0027C680;
extern void func_00123418(char *, int, int);
extern void func_0010DFD8(char *, int);
extern int func_00114BA8();
extern int func_0010E8A8(struct s_b0c0 *, int, int, struct req *, int, int *, int, void *, int);

int func_00114C30(int a0, int a1, int a2, int a3) {
    struct req *p;
    int r;

    if (D_00241D68 != 0)
        return D_00241D68;
    if (D_0027B0C0.f24 == 0)
        return -0x64;

    p = &D_0027B170;
    p->a = a0;
    p->b = a1;
    p->buf = D_0027B680;
    func_00123418(p->name, a2, 0x3FF);
    p->name[0x3FF] = 0;
    func_0010DFD8(D_0027B680, 0x400);
    r = func_0010E8A8(&D_0027B0C0, 0xC, 1, &D_0027B170, sizeof(struct req), &D_0027C680, 4, func_00114BA8, a3);
    if (r == 0)
        D_00241D68 = 0xC;
    return r;
}
