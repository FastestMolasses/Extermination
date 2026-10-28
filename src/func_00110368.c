// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 71.57% NEARMISS).
// Fixes read from the original instructions:
//  - The routine is the libkernel IOP reset (sceSifResetIop shape): the packet at
//    D_00279680 is a static, 64-byte-aligned SIF reset command (16-byte command
//    header with psize:8/dsize:24 bitfields, then arglen, mode and an 80-byte
//    argument string). The alignment makes gcc clear the dsize bitfield with a
//    64-bit read-modify-write, as the original does; the DMA descriptor is a typed
//    record {src, dest, size, attr} on the stack.
//  - sceSifGetReg takes an unsigned int register number and returns unsigned int;
//    with a plain int parameter gcc put the argument load into the call's delay
//    slot, which shifted the 8-byte-aligned copy loop (the old "hi-caching /
//    coloring wall" was this prototype plus byte-offset access).
//
// IOP reset: stops SIF DMA, reads the IOP's receive address (SIF register
// 0x80000000), stores mode, copies arg (without its terminator) into the packet
// counting its length, fills the header (dest 0, dsize 0, psize 0x68, command id
// 0x80000003), writes the packet back from the data cache (func_0010DFD8) and
// queues it with sceSifSetDma (attr 0x44). If the transfer was queued (non-zero id) it
// writes 0x10000 and then 0x20000 to SIF register 4 (the SM flag register), zeroes
// registers 0x80000002 and 0x80000000, and returns 1; otherwise returns 0.

typedef struct {
    unsigned int psize:8;
    unsigned int dsize:24;
    unsigned int dest;
    int cid;
    unsigned int opt;
} SifCmdHeader;

typedef struct {
    SifCmdHeader header;
    int arglen;
    int mode;
    char arg[80];
} SifResetPkt;

typedef struct {
    void *src;
    void *dest;
    int size;
    int attr;
} SifDmaTransfer;

extern int sceSifStopDma(void);
extern unsigned int sceSifGetReg(unsigned int a);
extern void func_0010DFD8(void *addr, int size);
extern int sceSifSetDma(void *p, int n);
extern int sceSifSetReg(int a, int b);

extern SifResetPkt D_00279680 __attribute__((aligned(64)));

int func_00110368(char *arg, int mode)
{
    SifDmaTransfer dmat;
    int arglen;
    unsigned int r;

    sceSifStopDma();
    r = sceSifGetReg(0x80000000);
    D_00279680.mode = mode;
    for (arglen = 0; arg[arglen] != 0; arglen++)
        D_00279680.arg[arglen] = arg[arglen];
    D_00279680.header.dest = 0;
    D_00279680.arglen = arglen;
    D_00279680.header.cid = 0x80000003;
    D_00279680.header.dsize = 0;
    D_00279680.header.psize = sizeof D_00279680;

    dmat.src = &D_00279680;
    dmat.dest = (void *)r;
    dmat.size = sizeof D_00279680;
    dmat.attr = 0x44;
    func_0010DFD8(&D_00279680, sizeof D_00279680);

    if (sceSifSetDma(&dmat, 1) != 0) {
        sceSifSetReg(4, 0x10000);
        sceSifSetReg(4, 0x20000);
        sceSifSetReg(0x80000002, 0);
        sceSifSetReg(0x80000000, 0);
        return 1;
    }
    return 0;
}
