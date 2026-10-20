// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Appends two tags to DMA list ch: a ref tag (id 0x30, qwc 1) for D_00275674,
// then, after recording addr in slot ch of the ref table (+0x50), a call tag
// (id 0x50, qwc 0) for addr.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 3, 2026-10-20).

typedef struct {
    unsigned short qwc;
    unsigned char pad;
    unsigned char id;
    int addr;
    int w2;
    int w3;
} DmaTag;
typedef struct {
    char pad[0x10];
    DmaTag *cur[16];
    int ref[16];
} VifLists;
extern VifLists *D_00275670;
extern int D_00275674;

void vif_append_ref_tag(int ch, int addr) {
    {
        DmaTag **pp = &D_00275670->cur[ch];
        int base = D_00275674;
        (*pp)->id = 0x30;
        (*pp)->addr = base;
        (*pp)->qwc = 1;
        (*pp)++;
    }
    D_00275670->ref[ch] = addr;
    {
        DmaTag **pp = &D_00275670->cur[ch];
        (*pp)->id = 0x50;
        (*pp)->addr = addr;
        (*pp)->qwc = 0;
        (*pp)++;
    }
}
