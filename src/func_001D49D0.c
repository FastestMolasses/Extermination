// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// render_vif: builds a VIF unpack/ref-tag DMA chain. Loads the global render
// context pointer D_00275670, computes a tag pointer D_00816940 + (ctx[0x9C]<<7),
// then writes the active packet header at *(ctx+0x10) (qword count=0x30, ptr field,
// size=8), advances the packet write cursor by 0x10, and kicks off the follow-up
// builders. Key match lever: the tag-address compute (a3 = base + (n<<7)) must be
// hoisted to its own statement BEFORE the header stores so mwcc schedules the
// lw ctx[0x9C] early (idiom 12/15); this is the regalloc/schedule tie-break the
// pinned 991202 build loses but mwcc 2.3.3 (000906) and mwcc 2.4 win.
extern void vif_build_unpack_const(int a0);
extern void vif_append_ref_tag(int a0, void *a1);
extern void func_001D1F80(int a0, int a1, int a2);
extern void func_001D1FF0(int a0, int a1);
extern void func_001D6BA0(int a0, int a1, int a2, int a3, int t0, int t1);
extern void func_001D7100(int a0, int a1, void *a2, int a3);
extern char D_0023E8A0[];
extern char D_00816940[];
extern char *D_00275670;
extern int D_00275684;

void func_001D49D0(void) {
    char *t1;
    char *a3;
    vif_build_unpack_const(0);
    vif_append_ref_tag(0, D_0023E8A0);
    t1 = D_00275670;
    a3 = D_00816940 + (*(int *)(t1 + 0x9C) << 7);
    *(char *)(*(int *)(t1 + 0x10) + 0x3) = 0x30;
    *(int *)(*(int *)(t1 + 0x10) + 0x4) = (int)a3;
    *(short *)(*(int *)(t1 + 0x10) + 0x0) = 8;
    *(int *)(t1 + 0x10) = *(int *)(t1 + 0x10) + 0x10;
    func_001D1F80(0, 2, 6);
    func_001D1FF0(0, 0);
    func_001D6BA0(0, D_00275684, 7, 7, 0, 1);
    func_001D7100(0, 4, D_00275670 + 0x24B0, 0x40);
}
