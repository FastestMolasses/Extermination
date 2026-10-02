// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Queues the static draw program for the current frame buffer: a VIF unpack
// header (vif_build_unpack_const(0)), a DMA ref tag to the program at
// D_00239C90, then a DMA ref tag at the render context's packet cursor
// (D_00275670 + 0x10): id byte 0x30 (ref), address D_00816440 + the context's
// +0x9C index * 0x80, qwc 8; the cursor advances one quadword.
extern char *D_00275670;
extern char D_00816440[0x10000];
extern char D_00239C90[8];
extern void vif_build_unpack_const(int a0);
extern void vif_append_ref_tag(int a0, char *a1);

void func_001D4960(void) {
    char *ctx;
    char *addr;

    vif_build_unpack_const(0);
    vif_append_ref_tag(0, D_00239C90);
    ctx = D_00275670;
    addr = D_00816440 + (*(int *)(ctx + 0x9C) << 7);
    (*(char **)(ctx + 0x10))[3] = 0x30;
    *(char **)(*(char **)(ctx + 0x10) + 4) = addr;
    *(short *)*(char **)(ctx + 0x10) = 8;
    *(char **)(ctx + 0x10) += 0x10;
}
