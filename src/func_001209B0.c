// COMPILER: eegcc
// CFLAGS: -O2
// Walk the chain at a0+0x1D8; for each active entry (fC != 0) invoke the
// callback and OR its result into the accumulator.
struct entry {
    char pad[0xC];
    short fC;       // 0xC
};
struct blk {
    struct blk *next;   // 0x0
    int count;          // 0x4
    struct entry *ents; // 0x8
};

int func_001209B0(int a0, int (*cb)(struct entry *)) {
    int acc = 0;
    struct blk *b = (struct blk *)(a0 + 0x1D8);
    if (b != 0) {
        do {
            int i = b->count;
            struct entry *e = b->ents;
            for (i = i - 1; i >= 0; i--) {
                if (e->fC != 0) {
                    acc |= cb(e);
                }
                e = (struct entry *)((char *)e + 0x58);
            }
            b = b->next;
        } while (b != 0);
    }
    return acc;
}
