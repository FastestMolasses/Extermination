// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Twin of func_001D40E0 with its own globals (D_00275680, D_0026E4C0/D0,
// D_00251C10/20/24/28) and func_001D1F80(arg0, 1, 7) in place of (arg0, 1, 1).
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 4, 2026-10-20). The
// 96.55 NEARMISS came from the same wrong prototypes as func_001D40E0.

extern char D_0023D0C0[16];
extern unsigned char D_002514B0[16];
extern char D_00251C10[16];
extern int D_00251C20[2];
extern int D_00251C24[2];
extern int D_00251C28[2];
extern char D_0026E4C0[16];
extern char D_0026E4D0[16];
extern int D_00275670;
extern int D_00275680;
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

void func_001D4440(int arg0, char *arg1, float *arg2) {
    char **slot;
    int v;
    unsigned char *dma;

    vif_append_ref_tag(arg0, D_0023D0C0);
    func_001D6930(arg0, D_00275680, 8, 8, D_0026E4C0);
    func_001D1F80(arg0, 0, 2);
    D_00251C20[0] = float_to_int(128.0f * arg2[0]);
    D_00251C24[0] = float_to_int(128.0f * arg2[1]);
    D_00251C28[0] = float_to_int(128.0f * arg2[2]);
    func_001D6580(arg0, D_0026E4D0, D_00251C10, 0x48);
    func_001D1F20(arg0);
    func_001D1F80(arg0, 1, 7);
    func_001D6BA0(arg0, D_00275680, 8, 8, 0, 0);
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
