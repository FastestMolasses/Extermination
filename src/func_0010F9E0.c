// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 57.15% NEARMISS).
// Fixes read from the original instructions:
// - The "codegen-shape wall" was a wrong body: the request is a record of a 4-byte mode
//   word followed by a 0xFC-byte name, and the copy is an ordinary bounded loop
//   `for (i = 0; i < 0xFC; i++) if ((name[i] = src[i]) == 0) break;` (ee-gcc rotates
//   it, which is where the extra instructions came from), with plain-char elements.
// - The overflow case (i == 0xFC) terminates the last name byte and clamps the length
//   to 0xFB; the mode word is stored after that test, then the last name byte is
//   always cleared again.
// - The ninth argument of the RPC call is the 0 passed on the stack.
//
// libkernel SIF RPC client call: if the client latch D_00241C90 is negative (not
// bound) it returns 0. Otherwise it copies the NUL-terminated name (at most 0xFB
// characters) and the mode word into the send buffer D_00279340 and issues
// func_0010E8A8 (SifCallRpc) on the client block D_00279240 with rpc number 3,
// mode 0, send size = name length + 5, and a 4-byte receive buffer D_00279280.
// A failed call returns -1; otherwise the server's 4-byte reply is returned.

extern int D_00241C90;

struct name_req {
    int mode;
    char name[0xFC];
};
extern struct name_req D_00279340;
extern int D_00279240;
extern int D_00279280;

extern int func_0010E8A8(void *client, int rpc_number, int mode, void *send, int ssize,
                         void *recv, int rsize, int endfunc, int efarg);

int func_0010F9E0(const char *name, int mode)
{
    int i;

    if (D_00241C90 < 0)
        return 0;

    for (i = 0; i < 0xFC; i++) {
        if ((D_00279340.name[i] = name[i]) == 0)
            break;
    }
    if (i == 0xFC) {
        D_00279340.name[0xFB] = 0;
        i = 0xFB;
    }
    D_00279340.mode = mode;
    D_00279340.name[0xFB] = 0;
    if (func_0010E8A8(&D_00279240, 3, 0, &D_00279340, i + 5, &D_00279280, 4, 0, 0) < 0)
        return -1;
    return D_00279280;
}
