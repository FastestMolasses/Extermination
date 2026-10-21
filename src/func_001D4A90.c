// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Streams the data after the 0x40-byte header at src to DMA list ch as ref
// tags (id 0x30): each covers n = min(left, 0x1F8) units (qwc n * 0x82, address
// advanced by n * 0x820 bytes), and left -= 0x1F8 until it is <= 0; left starts
// at src[0]. Instruction-for-instruction twin of func_001D4F30.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 4, 2026-10-20; was
// 98.91 NEARMISS). Lever: store the qwc before advancing the address, so the
// qwc shift is formed first from the shared n * 65.

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

void func_001D4A90(int ch, int *src) {
    DmaTag **pp;
    char *run;
    int n;
    int chunk;

    n = src[0];
    run = (char *)src + 0x40;
    while (n > 0) {
        if (n < 0x1F8) {
            chunk = n;
        } else {
            chunk = 0x1F8;
        }
        n -= 0x1F8;
        pp = &D_00275670->cur[ch];
        (*pp)->id = 0x30;
        (*pp)->addr = (int)run;
        (*pp)->qwc = chunk * 0x82;
        run += chunk * 0x820;
        (*pp)++;
    }
}
