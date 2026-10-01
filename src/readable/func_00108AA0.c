// NEARMISS func_00108AA0  (vram 0x00108AA0, 0x2F0 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 65.91% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc layout of the nested scan loops and callback paths (structure and calls follow the
// original; not iterated).
//
// The function links from the asm body in src/func_00108AA0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

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

typedef struct DemuxEvent {
    int type;
    void *data;
    void *data_end;
    int len;
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
extern void *func_001087E8(BitReader *br, int bitpos);
extern void func_00108EA8(BitReader *br, PesInfo *pkt);
extern void func_00109068(BitReader *br, unsigned long long *pes);

int func_00108AA0(char *m, unsigned char *data, int len, unsigned char *ring, int ringsize) {
    BitReader br;
    PesInfo pkt;
    DemuxEvent ev;
    char *tab = *(char **)(m + 0x40);
    CbEntry *cbs = *(CbEntry **)(tab + 0x44);
    DemuxCb deflt = 0;
    void *deflt_arg = 0;
    int consumed = 0;
    int keep = 1;
    unsigned int limit;
    int i;

    func_00108608(&br, data, ring, ringsize);
    for (i = 0; i < *(int *)(tab + 0x48); i++) {
        if (cbs[i].key == 0xBDFF000000ULL) {
            deflt_arg = cbs[i].arg;
            deflt = cbs[i].func;
        }
        if (deflt != 0) {
            break;
        }
    }
    limit = len << 3;
    do {
        if (func_00108640(&br, 32) == 0x1BA) {
            func_00108EA8(&br, &pkt);
        }
        while (func_00108640(&br, 24) == 1 && func_00108640(&br, 32) != 0x1BA
               && func_00108640(&br, 32) != 0x1B9 && br.pos < limit && keep) {
            func_00109068(&br, &pkt.key);
            if (limit < br.pos) {
                continue;
            }
            for (i = 0; i < *(int *)(tab + 0x48); i++) {
                if (cbs[i].key == (pkt.key & cbs[i].mask)) {
                    ev.type = 6;
                    ev.data = func_001087E8(&br, pkt.start);
                    ev.data_end = func_001087E8(&br, pkt.end);
                    ev.pts = pkt.pts;
                    ev.len = pkt.len;
                    ev.dts = pkt.dts;
                    keep = cbs[i].func(m, &ev, cbs[i].arg);
                    break;
                }
            }
            if (i == *(int *)(tab + 0x48) && deflt != 0) {
                ev.type = 6;
                ev.data = func_001087E8(&br, pkt.start);
                ev.data_end = func_001087E8(&br, pkt.end);
                ev.pts = pkt.pts;
                ev.len = pkt.len;
                ev.dts = pkt.dts;
                keep = deflt(m, &ev, deflt_arg);
            }
            if (keep) {
                consumed = (int)(br.pos >> 3);
            }
        }
    } while (!(limit < br.pos) && func_00108640(&br, 32) == 0x1BA);
    return consumed;
}
