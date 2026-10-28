// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 65.91% NEARMISS
// companion in src/readable/). Fixes read from the original instructions:
//  - the default-callback search tests "no default yet" in the for condition
//    ahead of the count test (the old in-body break kept ee-gcc from rotating
//    the loop);
//  - the outer pack loop is while (1) with one combined break test at its end
//    (the do-while placed the second test at the loop top);
//  - the bit limit is the expression len << 3 at each use (an unsigned local
//    added zero extensions);
//  - the reader and packet info are used through pointers; the reader pointer
//    is taken after the reader is initialised (func_00108608(&brs, ...)), which
//    decides where the original sets it up;
//  - types decide ee-gcc's alias analysis and so the store order: the event
//    type is an enum, the event's two addresses are unsigned char pointers
//    (callee func_001087E8 returns one), its length is unsigned; the PES parsers
//    func_00108EA8 / func_00109068 return int (as their definitions in src/ do).

// Movie library (SDK libmpeg): demultiplexes an MPEG program stream. The
// data starts at data inside the ring [ring, ring + ringsize) and len bytes
// are available. m+0x40 holds the callback table: +0x44 an array of +0x48
// 0x18-byte entries (stream key, key mask, callback, argument); the entry
// keyed 0xBDFF000000 is the default. A bit reader (func_00108608 / skip
// func_00108660 / peek func_00108640) walks the stream: a pack header
// (start code 0x1BA) is parsed by func_00108EA8; then while the next start
// code is a packet (prefix 1, not 0x1BA / 0x1B9), the reader is inside the
// data and the last callback asked to continue, func_00109068 parses the
// PES header into the packet info. A packet that lies wholly inside the
// data is handed to the first callback whose masked key matches its stream
// key, else to the default, as event 6 with the payload start and end
// addresses (func_001087E8), its length and its PTS / DTS; a non-zero
// result records the bytes consumed. Packs repeat while one follows within
// the data. Returns the bytes consumed by the last accepted packet.
typedef struct BitReader {
    unsigned long long bits;
    unsigned char *start;
    unsigned char *cur;
    unsigned int nbits;
    int pad14;
    unsigned long long pos;
    unsigned char *ring;
    unsigned char *end;
    int size;
    int pad2C;
} BitReader;

typedef struct PesInfo {
    char pack[0x18];            /* pack header fields */
    unsigned long long key;     /* +0x18: stream key */
    char pad20[8];
    long long pts;              /* +0x28 */
    long long dts;              /* +0x30 */
    int end;                    /* +0x38: payload end, in bits */
    int len;                    /* +0x3C */
    int start;                  /* +0x40: payload start, in bits */
    int pad44;
} PesInfo;

typedef enum DemuxEventType {
    DEMUX_EV_PACKET = 6
} DemuxEventType;

typedef struct DemuxEvent {
    DemuxEventType type;
    unsigned char *header;      /* payload start address */
    unsigned char *data;        /* payload end address */
    unsigned int len;
    long long pts;
    long long dts;
} DemuxEvent;

typedef int (*DemuxCb)(char *m, DemuxEvent *ev, void *arg);

typedef struct CbEntry {
    unsigned long long key;
    unsigned long long mask;
    DemuxCb func;
    void *arg;
} CbEntry;

extern void func_00108608(BitReader *br, unsigned char *start, unsigned char *ring, int size);
extern int func_00108640(BitReader *br, int n);
extern unsigned char *func_001087E8(BitReader *br, int bitpos);
extern int func_00108EA8(BitReader *br, PesInfo *pkt);
extern int func_00109068(BitReader *br, unsigned long long *pes);

int func_00108AA0(char *m, unsigned char *data, int len, unsigned char *ring, int ringsize) {
    BitReader brs;
    PesInfo pkts;
    DemuxEvent ev;
    DemuxCb deflt_func = 0;
    void *deflt_arg = 0;
    BitReader *br;
    PesInfo *pkt = &pkts;
    char *tab = *(char **)(m + 0x40);
    CbEntry *cbs = *(CbEntry **)(tab + 0x44);
    int consumed = 0;
    int keep = 1;
    int i;

    func_00108608(&brs, data, ring, ringsize);
    br = &brs;
    for (i = 0; deflt_func == 0 && i < *(int *)(tab + 0x48); i++) {
        if (cbs[i].key == 0xBDFF000000ULL) {
            deflt_func = cbs[i].func;
            deflt_arg = cbs[i].arg;
        }
    }
    while (1) {
        if (func_00108640(br, 32) == 0x1BA) {
            func_00108EA8(br, pkt);
        }
        while (func_00108640(br, 24) == 1 && func_00108640(br, 32) != 0x1BA
               && func_00108640(br, 32) != 0x1B9 && br->pos < (len << 3) && keep) {
            func_00109068(br, &pkt->key);
            if ((len << 3) < br->pos) {
                continue;
            }
            for (i = 0; i < *(int *)(tab + 0x48); i++) {
                if (cbs[i].key == (pkt->key & cbs[i].mask)) {
                    ev.type = DEMUX_EV_PACKET;
                    ev.header = func_001087E8(br, pkt->start);
                    ev.data = func_001087E8(br, pkt->end);
                    ev.pts = pkt->pts;
                    ev.len = pkt->len;
                    ev.dts = pkt->dts;
                    keep = cbs[i].func(m, &ev, cbs[i].arg);
                    break;
                }
            }
            if (i == *(int *)(tab + 0x48) && deflt_func != 0) {
                ev.type = DEMUX_EV_PACKET;
                ev.header = func_001087E8(br, pkt->start);
                ev.data = func_001087E8(br, pkt->end);
                ev.pts = pkt->pts;
                ev.len = pkt->len;
                ev.dts = pkt->dts;
                keep = deflt_func(m, &ev, deflt_arg);
            }
            if (keep) {
                consumed = (int)(br->pos >> 3);
            }
        }
        if ((len << 3) < br->pos || func_00108640(br, 32) != 0x1BA) {
            break;
        }
    }
    return consumed;
}
