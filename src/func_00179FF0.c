// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Per-frame bone-matrix update for a skeletal object `self`. Builds the object's
// root TRS matrix from the T/R/S blocks at self+0xD0/0xB0/0xC0/0x60, then for
// each of the self+0xC bones, concatenates the bone's local matrix (bone+0x90)
// against either its parent's matrix or the root: D_00275B40 is the bone-pointer
// table (one int pointer per bone, byte-strided by `off`); each bone's parent
// index lives at bone+0x64 (short, -1 == no parent / use root). Marks self+0x303
// = 1 (matrices-ready flag) at the end.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 91.1%. Keys: (1) D_00275B40 is a gp-rel pointer global (-sdatathreshold 8) and
// is indexed with explicit byte offsets (`D_00275B40 + off`, `+= 4`) to keep the
// target's single addu and avoid an extra induction register; (2) the parent
// index is `parent * 4` on the already sign-extended `int parent` (from the lh),
// reproducing the plain `sll v0,v1,2` and avoiding the dsll32/dsra32 64-bit
// re-sign-extension a short index would emit.
extern void build_trs_matrix(void *, void *, void *, void *);
extern void func_001026D0(unsigned char *, unsigned char *, unsigned char *);
extern unsigned char *D_00275B40;

void func_00179FF0(unsigned char *self) {
    int i;
    int off;
    unsigned char *bone;
    int parent;

    build_trs_matrix(self + 0xD0, self + 0xB0, self + 0xC0, self + 0x60);
    i = 0;
    off = 0;
    for (; i < *(unsigned char *)(self + 0xC); i++) {
        bone = *(unsigned char **)(D_00275B40 + off);
        parent = *(short *)(bone + 0x64);
        if (parent != -1) {
            func_001026D0(bone + 0x90, *(unsigned char **)(D_00275B40 + parent * 4) + 0x90, bone + 0x90);
        } else {
            func_001026D0(bone + 0x90, self + 0xD0, bone + 0x90);
        }
        off += 4;
    }
    *(unsigned char *)(self + 0x303) = 1;
}
