// NEARMISS func_001D4C30  (vram 0x001D4C30, 0x98 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.39% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Body corrected in decomp lane round 4: vif_build_unpack_const gets ch (the old C passed 0) and func_001D4A90 its real (ch, src) prototype. Residual is caller-saved temp coloring only, same instruction stream: the target never uses v0 after the second call, so each temp sits one register later (co...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Channel ch: vif_build_unpack_const(ch) (the original passes the channel; the
// earlier C passed 0), vif_append_ref_tag(ch, D_00239C90), one ref tag (id 0x30,
// qwc 8) for D_00816740 + frame * 128, then func_001D4A90(ch, src). Twin of
// func_001D4B80 (only the bank global differs).

typedef struct {
    unsigned short qwc;
    unsigned char pad;
    unsigned char id;
    int addr;
    int w2;
    int w3;
} DmaTag;
typedef struct {
    char pad[0x10];
    DmaTag *cur[16];
    int ref[16];
    int pad90[3];
    int frame;
} VifLists;
extern VifLists *D_00275670;
extern void vif_build_unpack_const(int ch);
extern void vif_append_ref_tag(int ch, char *addr);
extern char D_00239C90[];
extern char D_00816740[];
extern void func_001D4A90(int ch, int *src);

void func_001D4C30(int ch, int *src) {
    char *addr;
    DmaTag **pp;

    vif_build_unpack_const(ch);
    vif_append_ref_tag(ch, D_00239C90);
    addr = D_00816740 + D_00275670->frame * 128;
    pp = &D_00275670->cur[ch];
    (*pp)->id = 0x30;
    (*pp)->addr = (int)addr;
    (*pp)->qwc = 8;
    (*pp)++;
    func_001D4A90(ch, src);
}
