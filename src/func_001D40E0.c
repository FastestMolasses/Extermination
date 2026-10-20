// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// DMA list builder for channel arg0: vif_append_ref_tag(arg0, D_0023D0C0), the
// unpack setup func_001D6930 / func_001D1F80(arg0, 0, 2), three
// float_to_int(128 * pos[i]) words to D_00251BE0 / 4 / 8, func_001D6580,
// func_001D1F20, func_001D1F80(arg0, 1, 1), func_001D6BA0, func_001D1FF0(arg0, 3),
// then three ref tags (id 0x30): D_00817040 + frame * 128 (qwc 8); D_002514B0
// (qwc 2) only when func_001D2910(0) returns 0; arg1 + 0x40 (qwc = the word at
// arg1 + 4); finally func_001D1F20(arg0). Twin of func_001D4440.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 4, 2026-10-20). The
// 96.55 NEARMISS came from wrong prototypes (vif_append_ref_tag (ch, addr),
// func_001D2910 and func_001D1F20 one argument each).

extern char D_0023D0C0[16];
extern unsigned char D_002514B0[16];
extern char D_00251BD0[16];
extern int D_00251BE0[2];
extern int D_00251BE4[2];
extern int D_00251BE8[2];
extern char D_0026E470[16];
extern char D_0026E480[16];
extern int D_00275670;
extern int D_0027567C;
extern unsigned char D_00817040[16];

extern void vif_append_ref_tag(int ch, void *p);
extern int func_001D6930(int a0, int a1, int a2, int a3, char *t0);
extern void func_001D1F80(int ch, int a, int b);
extern int float_to_int(float f);
extern char *func_001D6580(int a0, char *a1, char *a2, int a3);
extern void func_001D1F20(int ch);
extern char *func_001D6BA0(int a0, int a1, int a2, int a3, int t0, int t1);
extern void func_001D1FF0(int ch, int idx);
extern int func_001D2910(int id);

void func_001D40E0(int arg0, char *arg1, float *arg2) {
    char **slot;
    int v;
    unsigned char *dma;

    vif_append_ref_tag(arg0, D_0023D0C0);
    func_001D6930(arg0, D_0027567C, 8, 8, D_0026E470);
    func_001D1F80(arg0, 0, 2);
    D_00251BE0[0] = float_to_int(128.0f * arg2[0]);
    D_00251BE4[0] = float_to_int(128.0f * arg2[1]);
    D_00251BE8[0] = float_to_int(128.0f * arg2[2]);
    func_001D6580(arg0, D_0026E480, D_00251BD0, 0x48);
    func_001D1F20(arg0);
    func_001D1F80(arg0, 1, 1);
    func_001D6BA0(arg0, D_0027567C, 8, 8, 0, 0);
    func_001D1FF0(arg0, 3);
    dma = D_00817040 + (*(int *)(D_00275670 + 0x9C) << 7);
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
