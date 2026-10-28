// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// SDK (SIF RPC, EE side): handler for the IOP's bind request (command
// 0x80000009, registered by func_0010E088). Takes a reply packet from the
// packet ring (func_0010E338), copies the request's packet address and client
// pointer into it, sets the command to 0x80000009, looks up the served
// function number in the server list (func_0010E5F8) and fills the server,
// its receive buffer and its callback buffer (all 0 when not found), then
// sends the 0x40-byte packet back with command 0x80000008 (func_0010DE78).
// Fixes read from the original instructions: the reply packet's two copied
// fields are words (unsigned int) while the request's are pointers, so the two
// loads are scheduled ahead of both stores; the packet address is copied first.

typedef struct {
    char hdr[0x14];
    void *pkt_addr;         /* 0x14 */
    int pad_18;
    void *client;           /* 0x1C */
    int sid;                /* 0x20 */
} SifRpcBindPkt;

typedef struct {
    char hdr[0x14];
    unsigned int pkt_addr;         /* 0x14 */
    int pad_18;
    unsigned int client;           /* 0x1C */
    unsigned int cid;       /* 0x20 */
    void *server;           /* 0x24 */
    void *buff;             /* 0x28 */
    void *cbuff;            /* 0x2C */
    char pad_30[0x10];
} SifRpcRendPkt;

typedef struct {
    int sid;                /* 0x00 */
    void *func;             /* 0x04 */
    void *buff;             /* 0x08 */
    int size;               /* 0x0C */
    void *cfunc;            /* 0x10 */
    void *cbuff;            /* 0x14 */
} SifRpcServerData;

extern SifRpcRendPkt *func_0010E338(void *rpc_data);
extern SifRpcServerData *func_0010E5F8(int sid, void *rpc_data);
extern int func_0010DE78(int cid, void *pkt, int size, int src, int dest, int opt);

void func_0010E648(SifRpcBindPkt *bind, void *data) {
    SifRpcRendPkt *rend;
    SifRpcServerData *sd;

    rend = func_0010E338(data);
    rend->pkt_addr = (unsigned int)bind->pkt_addr;
    rend->client = (unsigned int)bind->client;
    rend->cid = 0x80000009;
    sd = func_0010E5F8(bind->sid, data);
    if (sd == 0) {
        rend->server = 0;
        rend->buff = 0;
        rend->cbuff = 0;
    } else {
        rend->server = sd;
        rend->buff = sd->buff;
        rend->cbuff = sd->cbuff;
    }
    func_0010DE78(0x80000008, rend, 0x40, 0, 0, 0);
}
