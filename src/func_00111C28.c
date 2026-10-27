// COMPILER: eegcc
// CFLAGS: -O2 -fno-strength-reduce

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.26% NEARMISS).
// Fixes read from the original instructions:
//  - The "GPR coloring" residual was ee-gcc's allocation priority (references
//    weighted by loop depth over live length), so the source shape decides it
//    (same shape as the byte-matched sibling binder func_00113280):
//  - the bind-done latch reset (D_00241D20 = 0) sits INSIDE the retry loop,
//    just before the break, not after the loop;
//  - the delay loops are empty: the r5900 assembler pads short loops itself,
//    so the old inline nops were spurious and lengthened every live range;
//  - the request buffer is a struct (32-byte file record, 256-byte name, dest
//    pointer), so the name copy indexes from the name field;
//  - the debug print routine returns int (printf-like).

// libcdvd-style file search over SIF RPC: bind the search server once (retrying
// with a busy-wait), copy the name into the request, write back the cache, call,
// then copy the 32-byte file record back through the uncached view.

extern volatile int D_00241D08;
extern int D_00241CF8;
extern int D_00241D00;
extern int D_00241D20;
extern int D_00279B00;
extern int D_00279B08;
typedef struct {
    unsigned int lsn;
    unsigned int size;
    char name[16];
    unsigned char date[8];
} CdlFile;

typedef struct { CdlFile file; char name[0x100]; void *dest; } SearchReq;
extern SearchReq D_00279C80;
extern unsigned char D_00279DC0[];
typedef struct { int pad[9]; int server; int rest[30]; } RpcClient;
extern RpcClient D_00279E28;
extern char D_0026BB50[];
extern char D_0026BB70[];
extern char D_0026BB88[];
extern char D_0026BB98[];
extern char D_0026BBA8[];

extern void func_001118B8(void);
extern int PollSema(int sema);
extern void ReferThreadStatus(int a, void *b);
extern int func_00112D18(int a0);
extern void func_0010E088(int a0);
extern int func_0010E6F8(RpcClient *a0, unsigned int a1, int a2);
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int a4, void *a5,
                         int a6, void *a7, void *a8);
extern void func_0010DFD8(void *a0, int a1);
extern int func_00122B58(const char *fmt, ...);
extern int SignalSema(int sema);
extern void *memcpy(void *dst, const void *src, unsigned int n);

int func_00111C28(CdlFile *out, const char *name)
{
    char *req;
    int i;
    int d;
    int ret;

    func_001118B8();
    if (D_00241D08 != PollSema(D_00241D08)) {
        return 0;
    }

    D_00241D00 = 1;
    ReferThreadStatus(D_00279B00, &D_00279B08);
    if (func_00112D18(1) != 0) {
        SignalSema(D_00241D08);
        return 0;
    }

    func_0010E088(0);

    if (D_00241D20 < 0) {
        for (;;) {
            if (func_0010E6F8(&D_00279E28, 0x80000597u, 0) < 0) {
                if (D_00241CF8 > 0)
                    func_00122B58(D_0026BB50);
                d = 0x100000;
                do {
                    d--;
                } while (d != -1);
                continue;
            }
            if (D_00279E28.server != 0) {
                D_00241D20 = 0;
                break;
            }
            d = 0x100000;
            do {
                d--;
            } while (d != -1);
        }
    }

    i = 0;
    while (i < 0x100) {
        if ((D_00279C80.name[i] = name[i]) == 0)
            break;
        i++;
    }
    if (i == 0x100)
        D_00279C80.name[0xFF] = 0;

    req = (char *)&D_00279C80;
    *(char **)(req + 0x120) = req;

    if (D_00241CF8 > 0)
        func_00122B58(D_0026BB70, req + 0x20);

    func_0010DFD8(req, 0x124);

    if (func_0010E8A8(&D_00279E28, 0, 0, req, 0x124, D_00279DC0, 4, 0, 0) < 0) {
        SignalSema(D_00241D08);
        return 0;
    }

    memcpy((char *)out, (char *)((unsigned int)req | 0x20000000), 32);

    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BB88, out->name);
        if (D_00241CF8 > 0) {
            func_00122B58(D_0026BB98, out->size);
            if (D_00241CF8 > 0)
                func_00122B58(D_0026BBA8, out->lsn);
        }
    }

    ret = *(int *)((unsigned int)D_00279DC0 | 0x20000000);
    SignalSema(D_00241D08);
    return ret;
}
