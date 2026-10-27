// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 93.93% NEARMISS).
// Fixes read from the original instructions:
//  - The "D_FFFFF pseudo-symbol wall" and "coloring" residual were our bugs
//    (same shape as the byte-matched siblings func_00113280/func_00111C28):
//  - the delay loop and the bind-failure hang loop are empty: the r5900
//    assembler pads short loops itself, so the inline nops were spurious;
//  - the RPC client is a struct global (server field at +0x24) referenced
//    directly, not through a cached pointer local; that gives the original's
//    two copies of its address in saved registers;
//  - func_0010E088 returns nothing (void).

// sceMcInit-style init: sceSifInitRpc(0); retry-bind the memory-card server
// (sid 0x80000400) with a busy-wait between tries, printing and hanging if the
// bind call fails; then call fno 0xFE and check the server's returned
// versions (fail -120 / -121 with a message), clearing the bind on any error.

extern void func_0010E088(int);
extern int func_0010E6F8(void *, unsigned int, int);
extern int func_0010E8A8(void *, int, int, void *, int, void *, int, void *, void *);
extern int func_00122B58(const char *, ...);

typedef struct { int pad[9]; int server; int rest[30]; } RpcClient;
extern RpcClient D_0027B0C0;
extern unsigned char D_0027B140[];
extern int D_0027C680[];
extern char D_0026BED8[];
extern char D_0026BEF0[];
extern char D_0026BF18[];

int func_001140C8(void)
{
    int ret;
    int i;

    func_0010E088(0);

    for (;;) {
        if (func_0010E6F8(&D_0027B0C0, 0x80000400u, 0) < 0) {
            func_00122B58(D_0026BED8);
            for (;;) {
            }
        }
        if (D_0027B0C0.server != 0)
            break;
        i = 0x100000;
        do {
            i--;
        } while (i != 0);
    }

    ret = func_0010E8A8(&D_0027B0C0, 0xFE, 0, D_0027B140, 0x30, D_0027C680, 0xC, 0, 0);
    if (ret < 0) {
        D_0027B0C0.server = 0;
        return ret - 0x64;
    }
    if (D_0027C680[1] < 0x20A) {
        func_00122B58(D_0026BEF0);
        D_0027B0C0.server = 0;
        return -0x78;
    }
    if (D_0027C680[2] < 0x20E) {
        func_00122B58(D_0026BF18);
        D_0027B0C0.server = 0;
        return -0x79;
    }
    return D_0027C680[0];
}
