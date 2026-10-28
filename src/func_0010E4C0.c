// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 56.21% NEARMISS).
// Fixes read from the original instructions:
//  - The routine is the SIF RPC library's SifGetOtherData (command 0x8000000C, the
//    "rdata" request), written in the SDK's own shape: SifRpcReceiveData_t and an
//    other-data packet record (src/dest/size at 0x20/0x24/0x28, receive at 0x1C).
//  - The packet's rpc_id is read into a local before the two header stores into the
//    receive record (as in SifCallRpc, func_0010E8A8); this restores the plain
//    null-check branch with -1 in its delay slot.
//  - The test is `if (!(mode & 1))` with the semaphore (wait) path first and the
//    no-wait path in the else arm: the wait path is the fall-through in the original.
//
// SifGetOtherData — Sony SIF RPC library (libsifrpc), ee-gcc 2.9-991111-01 -O2.
// Asks the IOP to copy `size` bytes from IOP address `src` to EE address `dest`
// for the receive record `rd`. Returns -1 when no packet is free, -2 when the
// command cannot be sent, -3 when the semaphore cannot be created, else 0.
// Without the no-wait bit (mode & 1) it blocks on a fresh semaphore until the
// IOP answers. func_0010E270=_rpc_get_packet, func_0010DE38=SifSendCmd,
// D_00278BC0=_sif_rpc_data.
//
// Struct fields are `unsigned int` like SifCallRpc's records (one alias set keeps
// ee-gcc's stores in source order).

typedef struct t_ee_sema {
    int count;
    int max_count;
    int init_count;
    int wait_threads;
    unsigned int attr;
    unsigned int option;
} ee_sema_t;

typedef struct t_SifRpcHeader {
    unsigned int pkt_addr;  /* 0x00 */
    unsigned int rpc_id;    /* 0x04 */
    int sema_id;            /* 0x08 */
    unsigned int mode;      /* 0x0C */
} SifRpcHeader_t;

typedef struct t_SifRpcReceiveData {
    SifRpcHeader_t hdr;     /* 0x00 */
    unsigned int src;       /* 0x10 */
    unsigned int dest;      /* 0x14 */
    int size;               /* 0x18 */
} SifRpcReceiveData_t;

typedef struct t_SifRpcPktHeader {
    unsigned int sifcmd[4];  /* 0x00 */
    unsigned int rec_id;     /* 0x10 */
    unsigned int pkt_addr;   /* 0x14 */
    unsigned int rpc_id;     /* 0x18 */
} SifRpcPktHeader_t;

typedef struct t_SifRpcOtherDataPkt {
    SifRpcPktHeader_t hdr;   /* 0x00 */
    unsigned int receive;    /* 0x1C */
    unsigned int src;        /* 0x20 */
    unsigned int dest;       /* 0x24 */
    int size;                /* 0x28 */
} SifRpcOtherDataPkt_t;

extern void *func_0010E270(void *qd);                   /* _rpc_get_packet */
extern int func_0010DE38(unsigned int cmd, void *packet, int packet_size,
                         void *src_extra, void *dest_extra, int size_extra);
extern int CreateSema(ee_sema_t *sema);
extern int WaitSema(int sema_id);
extern int DeleteSema(int sema_id);

extern void *D_00278BC0;                                /* _sif_rpc_data */
int func_0010E4C0(SifRpcReceiveData_t *rd, void *src, void *dest, int size, int mode)
{
    ee_sema_t sema;
    SifRpcOtherDataPkt_t *other;
    unsigned int rpc_id;

    other = (SifRpcOtherDataPkt_t *)func_0010E270(&D_00278BC0);
    if (other == 0)
        return -1;

    rpc_id = other->hdr.rpc_id;
    rd->hdr.pkt_addr = (unsigned int)other;
    rd->hdr.rpc_id = rpc_id;

    other->src = (unsigned int)src;
    other->dest = (unsigned int)dest;
    other->size = size;
    other->hdr.pkt_addr = (unsigned int)other;
    other->receive = (unsigned int)rd;

    if (!(mode & 1)) {
        sema.max_count = 1;
        sema.init_count = 0;
        rd->hdr.sema_id = CreateSema(&sema);
        if (rd->hdr.sema_id < 0)
            return -3;
        if (func_0010DE38(0x8000000c, other, 64, 0, 0, 0) == 0)
            return -2;
        WaitSema(rd->hdr.sema_id);
        DeleteSema(rd->hdr.sema_id);
    } else {
        rd->hdr.sema_id = -1;
        if (func_0010DE38(0x8000000c, other, 64, 0, 0, 0) == 0)
            return -2;
    }
    return 0;
}
