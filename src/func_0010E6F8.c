// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 56.27% NEARMISS).
// Fixes read from the original instructions:
// - The routine is SifBindRpc (libsifrpc), so it now uses the SDK's typed records
//   (SifRpcClientData_t, SifRpcBindPkt_t, ee_sema_t) in place of byte offsets.
// - Arm order: the blocking (semaphore) path is the then-arm of `!(mode & 1)` and
//   the no-wait path is the else-arm, each with its own early returns.
// - The client's rpc_id is copied from the packet before pkt_addr is stored. That
//   order keeps ee-gcc from filling the null-check branch with a branch-likely, which
//   was the supposed "compiler wall".
//
// SifBindRpc(cd, sid, mode): clears cd->command and cd->server, takes a packet from
// _rpc_get_packet(&_sif_rpc_data) (-1 if none), links client and packet, stores the
// server id and sends SIF command 0x80000009 (RPC bind, 64-byte packet). With
// mode bit 0 (no-wait) sema_id = -1 and it returns at once. Otherwise it creates a
// semaphore (max 1, init 0), returning -3 on failure, then waits on it and deletes it.
// A failed send returns -2; success returns 0.
// func_0010E270 = _rpc_get_packet, func_0010DE38 = SifSendCmd, D_00278BC0 = _sif_rpc_data.

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
typedef struct t_SifRpcClientData {
    SifRpcHeader_t hdr;                 /* 0x00 */
    unsigned int command;               /* 0x10 */
    unsigned int buff;                  /* 0x14 */
    unsigned int cbuff;                 /* 0x18 */
    unsigned int end_function;          /* 0x1C */
    unsigned int end_param;             /* 0x20 */
    unsigned int server;                /* 0x24 */
} SifRpcClientData_t;
typedef struct t_SifRpcPktHeader {
    unsigned int sifcmd[4];  /* 0x00 */
    unsigned int rec_id;     /* 0x10 */
    unsigned int pkt_addr;   /* 0x14 */
    unsigned int rpc_id;     /* 0x18 */
} SifRpcPktHeader_t;
typedef struct t_SifRpcBindPkt {
    SifRpcPktHeader_t hdr;   /* 0x00 */
    unsigned int client;     /* 0x1C */
    unsigned int sid;        /* 0x20 */
} SifRpcBindPkt_t;

extern void *func_0010E270(void *qd);                   /* _rpc_get_packet */
extern int func_0010DE38(unsigned int cmd, void *packet, int packet_size,
                         void *src_extra, void *dest_extra, int size_extra);
extern int CreateSema(ee_sema_t *sema);
extern int WaitSema(int sema_id);
extern int DeleteSema(int sema_id);
extern void *D_00278BC0;                                /* _sif_rpc_data */
int func_0010E6F8(SifRpcClientData_t *cd, int sid, int mode)
{
    ee_sema_t sema;
    SifRpcBindPkt_t *bind;

    cd->command = 0;
    cd->server = 0;
    bind = (SifRpcBindPkt_t *)func_0010E270(&D_00278BC0);
    if (bind == 0)
        return -1;
    cd->hdr.rpc_id = bind->hdr.rpc_id;
    cd->hdr.pkt_addr = (unsigned int)bind;
    bind->sid = sid;
    bind->hdr.pkt_addr = (unsigned int)bind;
    bind->client = (unsigned int)cd;

    if (!(mode & 0x01)) {
        sema.max_count = 1;
        sema.init_count = 0;
        cd->hdr.sema_id = CreateSema(&sema);
        if (cd->hdr.sema_id < 0)
            return -3;
        if (func_0010DE38(0x80000009, bind, 64, 0, 0, 0) == 0)
            return -2;
        WaitSema(cd->hdr.sema_id);
        DeleteSema(cd->hdr.sema_id);
    } else {
        cd->hdr.sema_id = -1;
        if (func_0010DE38(0x80000009, bind, 64, 0, 0, 0) == 0)
            return -2;
    }
    return 0;
}
