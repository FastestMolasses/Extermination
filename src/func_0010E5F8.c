// COMPILER: eegcc
// CFLAGS: -O2
// Nested list search: over buckets (a1->f28, chained at f14), scan entries
// (f8, chained at f38) for the one whose f0 == a0; return it (or 0).
struct ent {
    int f0;             // 0x0
    char pad[0x34];
    struct ent *f38;    // 0x38
};
struct bucket {
    char pad8[0x8];
    struct ent *f8;     // 0x8
    char pad14[0x14 - 0xC];
    struct bucket *f14; // 0x14
};
struct root {
    char pad28[0x28];
    struct bucket *f28; // 0x28
};

struct ent *func_0010E5F8(int a0, struct root *a1in) {
    struct bucket *b = a1in->f28;
    while (b != 0) {
        struct ent *e = b->f8;
        while (e != 0) {
            if (e->f0 == a0) {
                return e;
            }
            e = e->f38;
        }
        b = b->f14;
    }
    return 0;
}
