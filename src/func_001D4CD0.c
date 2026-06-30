// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Sibling of func_001D49D0: builds a VIF unpack/ref-tag DMA chain. After
// vif_build_unpack_const(0) / vif_append_ref_tag(0,&D_0023C200) and two
// sub-builders, loads the global render context D_00275670, computes the
// tag address D_00816940 + (ctx[0x9C]<<7), writes the active packet header
// at *(ctx+0x10) (qcount=0x30, ptr field, size=8), advances the write
// cursor by 0x10, then kicks the follow-up builders func_001D6BA0 /
// func_001D7100. Tag-address compute hoisted to its own statement so mwcc
// 2.3.3 schedules the lw ctx[0x9C] early (the 991202 build loses it).
extern void vif_build_unpack_const(int a0);
extern void vif_append_ref_tag(int a0, void *a1);
extern void func_001D1F80(int a0, int a1, int a2);
extern void func_001D1FF0(int a0, int a1);
extern void func_001D6BA0(int a0, int a1, int a2, int a3, int t0, int t1);
extern void func_001D7100(int a0, int a1, void *a2, int a3);
extern char D_0023C200[];
extern char D_00816940[];
extern char *D_00275670;

void func_001D4CD0(void) {
    char *t3;
    char *t2;
    vif_build_unpack_const(0);
    vif_append_ref_tag(0, D_0023C200);
    func_001D1F80(0, 2, 6);
    func_001D1FF0(0, 0);
    t3 = D_00275670;
    t2 = D_00816940 + (*(int *)(t3 + 0x9C) << 7);
    *(char *)(*(int *)(t3 + 0x10) + 0x3) = 0x30;
    *(int *)(*(int *)(t3 + 0x10) + 0x4) = (int)t2;
    *(short *)(*(int *)(t3 + 0x10) + 0x0) = 8;
    *(int *)(t3 + 0x10) = *(int *)(t3 + 0x10) + 0x10;
    func_001D6BA0(0, 0x258000, 7, 7, 0, 1);
    func_001D7100(0, 8, D_00275670 + 0x24B0, 0x40);
}
