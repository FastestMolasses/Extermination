// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// DMA list builder for channel arg0: vif_append_ref_tag(arg0, D_0023C750),
// func_001D1F80(arg0, 0, 0), func_001D7080(arg0, -0x80, 0.0f), then two ref tags
// (id 0x30): D_00816840 + frame * 128 (qwc 8) and arg1 + 0x40 (qwc = the word at
// arg1 + 4, read as a word, not a halfword); finally func_001D1F80(arg0, 1, 0).
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 4, 2026-10-20). The
// 88.34 NEARMISS passed an invented fourth argument to the last func_001D1F80.

extern char D_0023C750[];
extern int D_00275670;
extern unsigned char D_00816840[16];

extern void vif_append_ref_tag(int ch, void *p);
extern void func_001D1F80(int ch, int a, int b);
extern void func_001D7080(int ch, int a, float f);

void func_001D4650(int arg0, char *arg1) {
    char **slot;
    int v;
    unsigned char *dma;

    vif_append_ref_tag(arg0, D_0023C750);
    func_001D1F80(arg0, 0, 0);
    func_001D7080(arg0, -0x80, 0.0f);
    dma = D_00816840 + (*(int *)(D_00275670 + 0x9C) << 7);
    slot = (char **)((D_00275670 + arg0 * 4) + 0x10);
    *(char *)(*slot + 3) = 0x30;
    *(unsigned char **)(*slot + 4) = dma;
    *(short *)(*slot + 0) = 8;
    *slot = *slot + 0x10;
    v = *(int *)(arg1 + 4);
    slot = (char **)((D_00275670 + arg0 * 4) + 0x10);
    *(char *)(*slot + 3) = 0x30;
    *(char **)(*slot + 4) = arg1 + 0x40;
    *(short *)(*slot + 0) = v;
    *slot = *slot + 0x10;
    func_001D1F80(arg0, 1, 0);
}
