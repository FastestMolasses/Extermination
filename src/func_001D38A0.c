// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Appends a ref tag (id 0x30, qwc 8) for D_00816440 + frame * 128 to DMA list
// ch, then tail-calls func_001D37D0(ch, arg1).
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
extern char D_00816440[];
extern void func_001D37D0(int ch, char *arg1);

void func_001D38A0(int ch, char *arg1) {
    char *addr = D_00816440 + D_00275670->frame * 128;
    DmaTag **pp = &D_00275670->cur[ch];
    (*pp)->id = 0x30;
    (*pp)->addr = (int)addr;
    (*pp)->qwc = 8;
    (*pp)++;
    func_001D37D0(ch, arg1);
}
