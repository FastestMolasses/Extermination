// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 76.20% NEARMISS).
// Fixes read from the original instructions:
//  - func_0010FAD0 takes no arguments (its definition in src/); the fourth parameter is
//    the RPC function number (an int), not a pointer.
//  - The SIF RPC call's argument order was wrong: it is (client, fno, 0, block, 0x200,
//    block, 0x10, 0, 0) — send buffer, send size, receive buffer, receive size.
//  - The tail was a wrong body: after a successful call the reply's first word is the
//    status; zero returns 0xFFFEFFFD, otherwise the status and the reply's second word
//    are stored to out[0] and out[1] and the function returns 0.
//  - The name buffer (D_00279448) is held in its own local; the request block starts
//    8 bytes before it.
//
// Libmc/libcdvd-family SDK RPC wrapper: makes sure the RPC client is bound
// (func_0010FAD0; failure -> 0xFFFF0000), refuses while sub__9PsIIlibkernl2000_4 reports
// busy (0xFFFEFFFC), copies two names of up to 0xFC bytes into the 0x200-byte request
// block (NUL-terminated at +0x103 and +0x1FF), then calls func_0010E8A8 (sceSifCallRpc)
// with that block as both send and receive buffer; an RPC failure returns 0xFFFEFFFF.
extern int func_0010FAD0(void);
extern int sub__9PsIIlibkernl2000_4(void);
extern void func_00123418(char *dst, char *src, int n);
extern int func_0010E8A8(char *bd, int fno, int mode, void *send, int ssize, void *recv,
                         int rsize, int end, int end_param);

extern char D_00279448[];
extern char D_00279640[];

int func_00110048(char *a0, char *a1, int *a2, int a3) {
    char *name;
    int *blk;
    int r;

    if (func_0010FAD0() < 0) {
        return 0xFFFF0000;
    }
    if (sub__9PsIIlibkernl2000_4() != 0) {
        return 0xFFFEFFFC;
    }

    name = D_00279448;
    blk = (int *)(name - 8);
    func_00123418(name, a0, 0xFC);
    ((char *)blk)[0x103] = 0;
    func_00123418(name + 0xFC, a1, 0xFC);
    ((char *)blk)[0x1FF] = 0;

    if (func_0010E8A8(D_00279640, a3, 0, blk, 0x200, blk, 0x10, 0, 0) < 0) {
        return 0xFFFEFFFF;
    }
    r = blk[0];
    if (r == 0) {
        return 0xFFFEFFFD;
    }
    a2[0] = r;
    a2[1] = blk[1];
    return 0;
}
