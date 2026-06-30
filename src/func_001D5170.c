// NEARMISS func_001D5170  (vram 0x001D5170, 0x170 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 78.61% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-allocation permutation (s-reg coloring of arg0/dst/savedip + cached gp-ptr temp t0 vs t1); body/structure + gp-rel-vs-absolute addressing correct -> permuter, not the clean-store nop
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS 78.6% (mwcc 2.3.3; pinned 991202 = 68.2%). Logic fully recovered and
// the gp-rel/absolute addressing now matches (D_00275670 stays gp-rel; the large
// data blobs D_00237720/D_00816A40 declared as arrays so they address absolutely
// like the target). Residual is a register-allocation permutation: the target
// keeps {arg0->s0, dst->s1, savedip->s2} and caches the D_00275670 ptr in t0/a2/a3
// per region; mwcc colors {arg0->s2, dst->s0, savedip->s1} and uses t1 for the
// cached ptr. Same instruction stream, permuted registers -> permuter.
//
// Builds a VIF DMA chain (via vif_build_unpack_const / vif_append_ref_tag): writes
// a 0x30-cmd ref tag pointing at D_00816A40 + (D_00275670->0x9C << 7), then emits
// one 0x30 unpack tag per 0x1F8-vertex chunk of arg0->[0], advancing dst by
// chunk*0x820 and writing the qwc as chunk*0x82, then a terminating 0x60 tag, and
// finally func_001CAAC0 with arg0->{0x34,0x38,0x3C} + 1.0f packed on the stack.
extern int vif_build_unpack_const(int);
extern int vif_append_ref_tag(int, void *);
extern int func_001D1F80(int, int, int, void *);
extern int func_001CAAC0(float *, void *, int, void *);
extern char D_00237720[256];
extern char D_00816A40[256];
extern char *D_00275670;

void func_001D5170(char *arg0) {
    int sp4C;
    float sp40, sp44, sp48;
    int n;
    int chunk;
    char *dst;
    void *savedip;
    char *base;
    char *g;

    savedip = *(void **)(D_00275670 + 0x1C);
    dst = arg0 + 0x40;
    vif_build_unpack_const(3);
    vif_append_ref_tag(3, D_00237720);
    g = D_00275670;
    *(char *)(*(char **)(g + 0x1C) + 3) = 0x30;
    base = D_00816A40 + (*(int *)(g + 0x9C) << 7);
    *(char **)(*(char **)(g + 0x1C) + 4) = base;
    *(short *)(*(char **)(g + 0x1C) + 0) = 8;
    *(char **)(g + 0x1C) = *(char **)(g + 0x1C) + 0x10;
    func_001D1F80(3, 2, 1, base);
    n = *(int *)(arg0 + 0);
    if (n > 0) {
        do {
            chunk = 0x1F8;
            if (n < 0x1F8) {
                chunk = n;
            }
            n -= 0x1F8;
            g = D_00275670;
            *(char *)(*(char **)(g + 0x1C) + 3) = 0x30;
            *(char **)(*(char **)(g + 0x1C) + 4) = dst;
            dst += chunk * 0x820;
            *(short *)(*(char **)(g + 0x1C) + 0) = (short)(chunk * 0x82);
            *(char **)(g + 0x1C) = *(char **)(g + 0x1C) + 0x10;
        } while (n > 0);
    }
    g = D_00275670;
    *(char *)(*(char **)(g + 0x1C) + 3) = 0x60;
    *(int *)(*(char **)(g + 0x1C) + 4) = 0;
    *(short *)(*(char **)(g + 0x1C) + 0) = 0;
    *(char **)(g + 0x1C) = *(char **)(g + 0x1C) + 0x10;
    sp40 = *(float *)(arg0 + 0x34);
    sp44 = *(float *)(arg0 + 0x38);
    sp48 = *(float *)(arg0 + 0x3C);
    sp4C = 0x3F800000;
    func_001CAAC0(&sp40, savedip, 0x60, D_00275670);
}
