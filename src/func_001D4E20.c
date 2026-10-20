// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Runs vif_build_unpack_const(ch) and vif_append_ref_tag(ch, D_00237180), then
// appends a ref tag (qwc 8) for D_00816640 + frame * 128.
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
extern char D_00816640[];
extern char D_00237180[];
extern void vif_build_unpack_const(int a0);
extern void vif_append_ref_tag(int ch, char *addr);

void func_001D4E20(int ch) {
    char *addr;
    DmaTag **pp;

    vif_build_unpack_const(ch);
    vif_append_ref_tag(ch, D_00237180);
    addr = D_00816640 + D_00275670->frame * 128;
    pp = &D_00275670->cur[ch];
    (*pp)->id = 0x30;
    (*pp)->addr = (int)addr;
    (*pp)->qwc = 8;
    (*pp)++;
}
