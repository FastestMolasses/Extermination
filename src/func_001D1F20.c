// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Appends a ref tag (id 0x30, qwc 0x19) for D_00275674 + frame * 400 + 0x20 to
// DMA list ch (frame = D_00275670 +0x9C).
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
    int pad90[3];
    int frame;
} VifLists;
extern VifLists *D_00275670;
extern int D_00275674;
void func_001D1F20(int ch) {
    int addr = D_00275674 + D_00275670->frame * 400 + 0x20;
    DmaTag **pp = &D_00275670->cur[ch];
    (*pp)->id = 0x30;
    (*pp)->addr = addr;
    (*pp)->qwc = 0x19;
    (*pp)++;
}
