// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 43.91% NEARMISS
// companion in src/readable/). Fixes read from the original instructions:
//  - the switch has a third, empty case, 0x8000000C (the RPC read-data reply):
//    with three case labels ee-gcc builds the binary decision tree the original
//    shows (equal-to-0x8000000A test, an unsigned greater-than test to the
//    end, then the 0x80000009 test); with two labels it emits a plain chain.
//  - the client record is re-read from the packet at the top of each case and
//    once more after the switch (the original reloads it after the end
//    function's call), with the packet and client typed as structs.

// SDK (SIF RPC client): completion callback of an RPC reply packet. Command
// 0x80000009 (bind reply) copies the server handle and buffers into the
// client record; command 0x8000000A (call reply) runs the client's end
// function with its argument; 0x8000000C (read-data reply) does nothing extra.
// Then the client's semaphore is signalled from the interrupt context
// (iSignalSema) when valid, its packet is freed with func_0010E318 and the
// client's packet pointer is cleared.
extern int iSignalSema(int sema);
extern void func_0010E318(void *pkt);

typedef struct {
    void *pkt_addr;
    unsigned int rpc_id;
    int sema_id;
    unsigned int mode;
} sceSifRpcHeader;

typedef struct {
    sceSifRpcHeader hdr;
    unsigned int command;
    void *buff;
    void *cbuff;
    void (*end_function)(void *);
    void *end_param;
    void *serve;
} sceSifClientData;

typedef struct {
    unsigned int psize:8;
    unsigned int dsize:24;
    unsigned int dest;
    int cid;
    unsigned int opt;
    int rec_id;
    void *pkt_addr;
    unsigned int rpc_id;
    sceSifClientData *client;
    unsigned int command;
    void *serve;
    void *buff;
    void *cbuff;
} sceSifRpcRendPkt;

void func_0010E3A8(sceSifRpcRendPkt *request, void *data)
{
    sceSifClientData *cd;

    switch (request->command) {
    case 0x8000000A:
        cd = request->client;
        if (cd->end_function)
            cd->end_function(cd->end_param);
        break;
    case 0x80000009:
        cd = request->client;
        cd->serve = request->serve;
        cd->buff = request->buff;
        cd->cbuff = request->cbuff;
        break;
    case 0x8000000C:
        break;
    }
    cd = request->client;
    if (cd->hdr.sema_id >= 0)
        iSignalSema(cd->hdr.sema_id);
    func_0010E318(cd->hdr.pkt_addr);
    cd->hdr.pkt_addr = 0;
}
