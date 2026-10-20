// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// DMA list builder for channel arg0: vif_append_ref_tag(arg0, D_0023D4D0),
// func_001D1F80(arg0, 1, 1), func_001D6F60(arg0, GS register 0x2004818599422020,
// 0x80), func_001D1FF0(arg0, 1), then three ref tags (id 0x30) on the channel's
// list: D_00817140 + frame * 128 (qwc 8); D_002514B0 (qwc 2) only when
// func_001D2910(0) returns 0; arg1 + 0x40 (qwc = the word at arg1 + 4).
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 4, 2026-10-20). The
// 94.82 NEARMISS came from wrong prototypes: vif_append_ref_tag takes (ch, addr)
// and func_001D2910 takes one argument.

extern void vif_append_ref_tag(int ch, void *p);
extern void func_001D1F80(int a, int b, int c);
extern void func_001D6F60(int a, long long b, int c);
extern void func_001D1FF0(int a, int b);
extern int func_001D2910(int a);
extern char D_0023D4D0[];
extern unsigned char D_002514B0[16];
extern unsigned char D_00817140[16];
extern int D_00275670;

void func_001D42E0(int arg0, char *arg1) {
    char **slot;
    int v;
    unsigned char *dma;

    vif_append_ref_tag(arg0, D_0023D4D0);
    func_001D1F80(arg0, 1, 1);
    func_001D6F60(arg0, 0x2004818599422020LL, 0x80);
    func_001D1FF0(arg0, 1);
    dma = D_00817140 + (*(int *)(D_00275670 + 0x9C) << 7);
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
    *(char **)(*slot + 4) = arg1 + 0x40;
    *(short *)(*slot + 0) = v;
    *slot = *slot + 0x10;
}
