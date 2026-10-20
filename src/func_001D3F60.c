// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// DMA list builder for channel arg0: vif_append_ref_tag(arg0, D_0023CC50),
// func_001D1F80(arg0, 1, 0), func_001D6B10(arg0, D_00275678, 8, 8),
// func_001D6BA0(arg0, D_00275678, 8, 8, 2, 0), func_001D1FF0(arg0, 3), then three
// ref tags (id 0x30): D_00816F40 + frame * 128 (qwc 8); D_002514B0 (qwc 2) only
// when func_001D2910(0) returns 0; arg1 + 0x40 (qwc = the word at arg1 + 4);
// finally func_001D1F20(arg0).
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 4, 2026-10-20). The
// 88.09 NEARMISS came from unprototyped calls passing invented arguments to
// func_001D2910 and func_001D1F20.

extern char D_0023CC50[];
extern unsigned char D_002514B0[16];
extern int D_00275670;
extern int D_00275678;
extern unsigned char D_00816F40[16];

extern void vif_append_ref_tag(int ch, void *p);
extern void func_001D1F80(int ch, int a, int b);
extern char *func_001D6B10(int a0, int a1, int a2, int a3);
extern void func_001D1F20(int ch);
extern char *func_001D6BA0(int a0, int a1, int a2, int a3, int t0, int t1);
extern void func_001D1FF0(int ch, int idx);
extern int func_001D2910(int id);

void func_001D3F60(int arg0, char *arg1) {
    char **slot;
    int v;
    unsigned char *dma;

    vif_append_ref_tag(arg0, D_0023CC50);
    func_001D1F80(arg0, 1, 0);
    func_001D6B10(arg0, D_00275678, 8, 8);
    func_001D6BA0(arg0, D_00275678, 8, 8, 2, 0);
    func_001D1FF0(arg0, 3);
    dma = D_00816F40 + (*(int *)(D_00275670 + 0x9C) << 7);
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
    func_001D1F20(arg0);
}
