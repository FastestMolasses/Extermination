// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 93.56% NEARMISS).
// Fixes read from the original instructions (same shape as the byte-matched
// sibling binders func_00113280 / func_00111C28):
//  - the delay loops are empty: the r5900 assembler pads short loops itself,
//    so the old inline nops were spurious;
//  - the bind-done latch reset (D_00241D1C = 0) sits INSIDE the retry loop,
//    just before the break, not after the loop;
//  - the retry limit is a post-increment test that continues the loop
//    (retry++ < 0x11), with the give-up return after it;
//  - the RPC client is a struct global (RpcClient) used directly, its server
//    field tested for the bind; the debug print returns int and
//    func_0010E088 is void.

// libcdvd-family init: bind the cdvdfsv RPC server (sid 0x80000596), retrying
// with a busy-wait (debug print on bind failure, give up after 18 empty binds),
// then issue fno 1 with mode 1 and end callback func_001115D0 / D_00279C08 = 0xB.
// Returns 1 on success, 0 on failure.

extern void func_0010E088(int);
typedef struct { int pad[9]; int server; int rest[30]; } RpcClient;
extern int func_0010E6F8(RpcClient *, unsigned int, int);
extern int func_0010E8A8(void *, int, int, void *, int, void *, int, void *, void *);
extern int func_00122B58(const char *, ...);
extern void func_001115D0(void *);

extern int D_00241D1C;
extern int D_00241CF8;
extern RpcClient D_00279BE0;
extern int D_00279C08;
extern char D_0026BB30[];

int func_00111AE0(void)
{
    int retry;
    int i;

    func_0010E088(0);

    if (D_00241D1C < 0) {
        retry = 0;
        for (;;) {
            if (func_0010E6F8(&D_00279BE0, 0x80000596u, 0) < 0) {
                if (D_00241CF8 > 0)
                    func_00122B58(D_0026BB30);
                i = 0x100000;
                do {
                    i--;
                } while (i != -1);
                continue;
            }
            if (D_00279BE0.server != 0) {
                D_00241D1C = 0;
                break;
            }
            i = 0x100000;
            do {
                i--;
            } while (i != -1);
            if (retry++ < 0x11)
                continue;
            return 0;
        }
    }

    D_00279C08 = 0xB;
    if (func_0010E8A8(&D_00279BE0, 1, 1, 0, 0, 0, 0, func_001115D0,
                      &D_00279C08) < 0)
        return 0;
    return 1;
}
