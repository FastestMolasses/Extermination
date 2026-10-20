// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Channel 0: vif_build_unpack_const(0), vif_append_ref_tag(0, D_00237180),
// func_001D1F80(0, 1, 0), then one ref tag (id 0x30, qwc 8) for
// D_00816440 + frame * 128.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 4, 2026-10-20), in the
// DmaTag / VifLists form of the matched func_001D4E20 (was 98.91 NEARMISS).

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
extern void vif_build_unpack_const(int ch);
extern void vif_append_ref_tag(int ch, char *addr);
extern char D_00237180[];
extern char D_00816440[];
extern void func_001D1F80(int ch, int a, int b);

void func_001D4DA0(void) {
    char *addr;
    DmaTag **pp;

    vif_build_unpack_const(0);
    vif_append_ref_tag(0, D_00237180);
    func_001D1F80(0, 1, 0);
    addr = D_00816440 + D_00275670->frame * 128;
    pp = &D_00275670->cur[0];
    (*pp)->id = 0x30;
    (*pp)->addr = (int)addr;
    (*pp)->qwc = 8;
    (*pp)++;
}
