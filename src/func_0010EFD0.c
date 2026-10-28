// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 73.67% NEARMISS).
// Fixes read from the original instructions:
// - SifCallRpc (func_0010E8A8) takes nine arguments: the trailing end_function and
//   end_param (both 0) were missing; the original passes them in the last argument
//   register and on the stack.
// - The bind-retry delay is a plain counted-down register loop (`i = 0x100000;
//   while (i--);`), not a volatile counter; the nops in it are the assembler's
//   short-loop padding.
// - The 16-entry queue table is cleared with a typed record pointer loop, and the
//   reply word is copied with memcpy(.., 4) (unaligned word copy), not an int load.
//
// libkernel file-I/O client init (sceFsInit shape): SifInitRpc(0); bind the
// 0x80000001 file-I/O RPC server into the client record at D_00279210, retrying
// with a busy-wait until its server field is set (bind failure returns -1);
// clear the +0x4 word of each of the 16 request records at D_00279110; write back
// the 0x40-byte buffer at D_00279080; call RPC 0xFF synchronously with a 4-byte
// reply into D_00279040 (failure returns 0xFFFEFFFF); copy the reply word into
// D_00279238, set D_00241C88 = 1 and return 0.

typedef struct SifRpcClientData {
    unsigned int hdr[4];
    unsigned int command;
    unsigned int buff;
    unsigned int cbuff;
    unsigned int end_function;
    unsigned int end_param;
    unsigned int server;
} SifRpcClientData_t;

typedef struct FioQ {
    unsigned int f0;
    unsigned int f4;
    unsigned int f8;
    unsigned int fC;
} FioQ;

extern void func_0010E088(int mode);
extern int func_0010E6F8(SifRpcClientData_t *cd, unsigned int rpc_number, int mode);
extern void func_0010DFD8(void *ptr, int size);
extern int func_0010E8A8(SifRpcClientData_t *client, int rpc_number, int mode,
                         void *send, int ssize, void *receive, int rsize,
                         void (*end_function)(void *), void *end_para);

extern SifRpcClientData_t D_00279210;
extern FioQ D_00279110[16];
extern unsigned char D_00279040[];
extern unsigned char D_00279080[];
extern int D_00279238;
extern int D_00241C88;

extern void *memcpy(void *dst, const void *src, unsigned int n);

int func_0010EFD0(void)
{
    int i;
    FioQ *p;

    func_0010E088(0);

    while (1) {
        if (func_0010E6F8(&D_00279210, 0x80000001, 0) < 0)
            return -1;
        if (D_00279210.server != 0)
            break;
        i = 0x100000;
        while (i--)
            ;
    }

    for (p = D_00279110; p < &D_00279110[16]; p++)
        p->f4 = 0;

    func_0010DFD8(D_00279080, 0x40);

    if (func_0010E8A8(&D_00279210, 0xFF, 0, 0, 0, D_00279040, 4, 0, 0) < 0)
        return (int)0xFFFEFFFF;

    memcpy(&D_00279238, D_00279040, 4);
    D_00241C88 = 1;
    return 0;
}
