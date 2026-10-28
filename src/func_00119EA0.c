// NEARMISS func_00119EA0  (vram 0x00119EA0, 0x1CC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.13% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful (the
// old "forward branch-likely wall" verdict was wrong: the continue-shaped slot scan, as in the
// byte-matched sibling func_00119650, emits the three forward annulled branches).
// Remaining differences: the original keeps a copy of the 0xFFFF-tested entry in a
// second register (non-likely branch to the shared return) and shifts it logically
// (srl: an unsigned value), and it materialises its own constant 1 for the claim-block
// stores (ours reuses the bank-state compare's 1), which reorders the claim stores.
// 2026-10-28 (wave wX2): the table entry is read into sel itself and then halved.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2
//
// Starts sequence `a1`, sub-entry `a2` (both < 0x80) of sound bank `a0 & 0x7FFF` (< 0x80):
// publishes the bank header and its table (header + word at 0x1C) in D_00281AC0 (+0x10,
// +0x28), requires bank state 1, the 'SShd' magic (0x64685353) at header+0xC and a word at
// +0x20 other than -1, then walks the halfword table (count, then per-sequence offsets; an
// entry of 0xFFFF means absent). The first of the 48 Seq records with +0x2E, the +0x30 word
// and +0x34 all zero is claimed: active flags, data pointer, bank/sequence/sub ids, 0x1000
// volumes, tempo 0x3C and +0x1C = 0x1E0000 / tempo word; bit 0x8000 of a0 sets +0x42.
// Returns the record index, or -1.

typedef struct {
    unsigned char pad00[0x0C];
    unsigned char *p0C;
    unsigned char pad10[0x18 - 0x10];
    int f18;
    int f1C;
    unsigned char pad20[0x24 - 0x20];
    unsigned short f24;
    unsigned short f26;
    unsigned short f28;
    unsigned char pad2A[0x2E - 0x2A];
    unsigned short f2E;
    union {
        int w;                  /* both halfwords tested as one word */
        struct {
            unsigned short f30;
            unsigned short f32;
        } h;
    } u30;
    unsigned short f34;
    unsigned char pad36[0x42 - 0x36];
    unsigned short f42;
    unsigned short f44;
    unsigned char pad46[0x48 - 0x46];
    int f48;
    int f4C;
    unsigned char pad50[0x54 - 0x50];
    unsigned short f54;
    unsigned short f56;
    unsigned char pad58[0x78 - 0x58];
} Seq;

typedef struct {
    unsigned char pad00[0x10];
    unsigned char *p10;
    unsigned char pad14[0x28 - 0x14];
    unsigned char *p28;
} Song;

typedef struct {
    unsigned char pad00[0x3A];
    unsigned short f3A;
} Tempo;

typedef struct {
    int state;
    unsigned char *hdr;
    int size;
} Bank;

extern Seq D_0027E0C0[48];
extern Bank D_0027C6C0[];
extern Song D_00281AC0;
extern Tempo D_0027F740;

int func_00119EA0(int a0, int a1, int a2)
{
    Seq *e = D_0027E0C0;
    int id = a0 & 0x7FFF;
    int ret = -1;
    int i;
    int flag;
    unsigned char *hdr;
    unsigned char *blk;
    int x;
    int sel;

    if ((unsigned int)id < 0x80 && a1 >= 0 && a1 < 0x80 && a2 >= 0 && a2 < 0x80) {
        flag = 0;
        if (a0 & 0x8000)
            flag = 1;
        hdr = D_0027C6C0[id].hdr;
        D_00281AC0.p10 = hdr;
        blk = hdr + *(int *)(hdr + 0x1C);
        D_00281AC0.p28 = blk;
        if (D_0027C6C0[id].state == 1 && *(int *)(hdr + 0xC) == 0x64685353 &&
            *(unsigned int *)(hdr + 0x20) != 0xFFFFFFFF && *(unsigned short *)blk >= a1) {
            sel = ((unsigned short *)blk)[a1 + 1];
            if (sel != 0xFFFF) {
                sel = sel >> 1;
                if (((unsigned short *)blk)[sel] >= a2) {
                    blk = blk + ((unsigned short *)blk)[a2 + sel + 1];
                    for (i = 0; i < 48; i++, e++) {
                        if (e->f2E != 0) continue;
                        if (e->u30.w != 0) continue;
                        if (e->f34 != 0) continue;
                        e->f34 = 1;
                        e->p0C = blk;
                        e->f24 = id;
                        e->f28 = a1;
                        e->f26 = a2;
                        e->f44 = 0x1000;
                                        e->f54 = 0x3C;
                        e->u30.h.f32 = 1;
                        e->f18 = i;
                        e->f48 = e->f4C = 0x1000;
                        e->f1C = 0x1E0000 / D_0027F740.f3A;
                        if (flag == 1)
                            e->f42 = flag;
                        ret = i;
                        break;
                    }
                }
            }
        }
    }
    return ret;
}
