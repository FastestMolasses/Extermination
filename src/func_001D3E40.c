// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Submits a model's VIF packets on channel arg0: vif_append_ref_tag(arg0,
// D_0023C480), then three ref tags (id 0x30): D_00816440 + frame * 128 (qwc 8);
// D_002514B0 (qwc 2) only when func_001D2910(0) returns 0; model + 0x40 (qwc =
// the word at model + 4).
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 4, 2026-10-20). Lever:
// the model pointer is a local copied from the parameter with an explicit cast
// and declared first, so it takes s0 and the packet pointer s1 (a parameter used
// directly is colored after the locals, in s2).

extern void vif_append_ref_tag(int ch, void *p);
extern int func_001D2910(int id);
extern char D_0023C480[];
extern unsigned char D_002514B0[16];
extern unsigned char D_00816440[16];
extern int D_00275670;

void func_001D3E40(int arg0, int model) {
    char *arg1 = (char *)model;
    char *mesh;
    char **slot;
    int v;
    unsigned char *dma;

    mesh = arg1 + 0x40;
    vif_append_ref_tag(arg0, D_0023C480);
    dma = D_00816440 + (*(int *)(D_00275670 + 0x9C) << 7);
    slot = (char **)((D_00275670 + arg0 * 4) + 0x10);
    *(char *)(*slot + 3) = 0x30;
    *(unsigned char **)(*slot + 4) = dma;
    *(short *)(*slot + 0) = 8;
    *slot = *slot + 0x10;
    if (func_001D2910(0) == 0) {
        slot = (char **)((D_00275670 + arg0 * 4) + 0x10);
        *(char *)(*slot + 3) = 0x30;
        *(char **)(*slot + 4) = (char *)D_002514B0;
        *(short *)(*slot + 0) = 2;
        *slot = *slot + 0x10;
    }
    v = *(int *)(arg1 + 4);
    slot = (char **)((D_00275670 + arg0 * 4) + 0x10);
    *(char *)(*slot + 3) = 0x30;
    *(char **)(*slot + 4) = mesh;
    *(short *)(*slot + 0) = v;
    *slot = *slot + 0x10;
}
