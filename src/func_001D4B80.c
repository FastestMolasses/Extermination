// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// TU_STATIC: func_001D4A90
//
// Byte-matched (objdiff 100%), decomp lane round 8, with func_001D4A90
// compiled as a static function of this file (// TU_STATIC:, MATCHING_GUIDE
// "Decomp lane round 7"). The original compiler saw that callee earlier in the
// same translation unit and knew its register use, which decided the
// caller-saved temporaries here (the former NEARMISS residual, 95.39%: each
// temporary one register later). tools/decomp/tu_static.py removes the static
// copy after checking it equals the original func_001D4A90; the call links to
// that function's own unit. DmaTag / VifLists / D_00275670 come from the
// included file.
//
// Channel ch: vif_build_unpack_const(ch), vif_append_ref_tag(ch, D_00239C90),
// one ref tag (id 0x30, qwc 8) for D_00816640 + frame * 128, then
// func_001D4A90(ch, src). Twin of func_001D4C30 (only the bank global differs).

static void func_001D4A90(int ch, int *src);  /* same-TU callee, see TU_STATIC */
#include "src/func_001D4A90.c"
extern void vif_build_unpack_const(int ch);
extern void vif_append_ref_tag(int ch, char *addr);
extern char D_00239C90[];
extern char D_00816640[];

void func_001D4B80(int ch, int *src) {
    char *addr;
    DmaTag **pp;

    vif_build_unpack_const(ch);
    vif_append_ref_tag(ch, D_00239C90);
    addr = D_00816640 + D_00275670->frame * 128;
    pp = &D_00275670->cur[ch];
    (*pp)->id = 0x30;
    (*pp)->addr = (int)addr;
    (*pp)->qwc = 8;
    (*pp)++;
    func_001D4A90(ch, src);
}
