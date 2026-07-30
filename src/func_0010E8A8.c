// NEARMISS func_0010E8A8  (vram 0x0010E8A8, 0x1B4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.89% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler wall — ONE adjacent independent-store swap. Expected emits `sw v0,32(s1); sw s0,0(s1); sw v1,4(s1)` (end_param, hdr.pkt_addr, hdr.rpc_id); ee-gcc emits `... sw v1,4(s1); sw s0,0(s1)` — the two stores are swapped. Proven source-order-independent: four source permutations (vJ/v...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// SifCallRpc — Sony SIF RPC library (libsifrpc), ee-gcc 2.9-991111-01 -O2.
// NOTE: the splat syscall-stub names are mis-assigned in this region; the real
// routines are EndOfHeap=CreateSema, SignalSema=WaitSema, RFU063=DeleteSema.
// func_0010E270=_rpc_get_packet, func_0010DFD8=SifWriteBackDCache,
// func_0010DE38=SifSendCmd, D_00278BC0=_sif_rpc_data.
//
// Struct fields are all `unsigned int` on purpose: that gives every access one
// alias set, which is what forces ee-gcc's list scheduler to keep the stores in
// source order (mixed field types let it reorder them and cost ~14%).

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

typedef struct t_SifRpcCallPkt {
    SifRpcPktHeader_t hdr;              /* 0x00 */
    unsigned int client;                /* 0x1C */
    unsigned int rpc_number;            /* 0x20 */
    unsigned int send_size;             /* 0x24 */
    unsigned int receive;               /* 0x28 */
    unsigned int recv_size;             /* 0x2C */
    unsigned int rmode;                 /* 0x30 */
    unsigned int server;                /* 0x34 */
} SifRpcCallPkt_t;

extern void *func_0010E270(void *qd);                   /* _rpc_get_packet    */
extern void func_0010DFD8(void *ptr, int size);         /* SifWriteBackDCache */
extern int func_0010DE38(unsigned int cmd, void *packet, int packet_size,
                         void *src_extra, void *dest_extra, int size_extra);
extern int EndOfHeap(ee_sema_t *sema);                  /* CreateSema */
extern int SignalSema(int sema_id);                     /* WaitSema   */
extern int RFU063(int sema_id);                         /* DeleteSema */

extern void *D_00278BC0;                                /* _sif_rpc_data */

int func_0010E8A8(SifRpcClientData_t *client, int rpc_number, int mode,
                  void *send, int ssize, void *receive, int rsize,
                  void (*end_function)(void *), void *end_para)
{
    SifRpcCallPkt_t *packet;
    ee_sema_t sema;
    unsigned int rpc_id;
    unsigned int server;

    packet = (SifRpcCallPkt_t *)func_0010E270(&D_00278BC0);
    if (packet == 0)
        return -1;

    rpc_id = packet->hdr.rpc_id;
    client->hdr.pkt_addr = (unsigned int)packet;
    client->hdr.rpc_id = rpc_id;
    client->end_function = (unsigned int)end_function;
    client->end_param = (unsigned int)end_para;

    packet->rpc_number = rpc_number;
    packet->send_size = ssize;
    packet->receive = (unsigned int)receive;
    packet->recv_size = rsize;
    packet->hdr.pkt_addr = (unsigned int)packet;
    server = client->server;
    packet->client = (unsigned int)client;
    packet->server = server;

    if (!(mode & 0x02)) {
        if (ssize > 0)
            func_0010DFD8(send, ssize);
        if (rsize > 0 && send != receive)
            func_0010DFD8(receive, rsize);
    }

    if (mode & 0x01) {
        if (end_function == 0)
            packet->rmode = 0;
        else
            packet->rmode = 1;
        client->hdr.sema_id = -1;
        if (func_0010DE38(0x8000000a, packet, 64, send, (void *)client->buff, ssize) == 0)
            return -2;
    } else {
        sema.max_count = 1;
        sema.init_count = 0;
        client->hdr.sema_id = EndOfHeap(&sema);
        if (client->hdr.sema_id < 0)
            return -3;
        packet->rmode = 1;
        if (func_0010DE38(0x8000000a, packet, 64, send, (void *)client->buff, ssize) == 0)
            return -2;
        SignalSema(client->hdr.sema_id);
        RFU063(client->hdr.sema_id);
    }
    return 0;
}
