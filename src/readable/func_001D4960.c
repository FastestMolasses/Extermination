// NEARMISS func_001D4960  (vram 0x001D4960, 0x70 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 90.18% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Register allocation and scheduling of the cursor reloads: the original keeps the context in t0
// and builds the address after the first store; mwcc hoists the add (address and constant hoists
// measured).
//
// The function links from the asm body in src/func_001D4960.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
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
    char *q;
    char *addr;

    vif_build_unpack_const(0);
    vif_append_ref_tag(0, D_00239C90);
    ctx = D_00275670;
    addr = D_00816440 + (*(int *)(ctx + 0x9C) << 7);
    q = *(char **)(ctx + 0x10);
    q[3] = 0x30;
    q = *(char **)(ctx + 0x10);
    *(char **)(q + 4) = addr;
    q = *(char **)(ctx + 0x10);
    *(short *)q = 8;
    q = *(char **)(ctx + 0x10);
    *(char **)(ctx + 0x10) = q + 0x10;
}
