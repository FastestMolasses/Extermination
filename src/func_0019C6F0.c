// NEARMISS func_0019C6F0  (vram 0x0019C6F0, 0x140 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.50% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful.
// Remaining differences: the original copies the first record-list pointer into a
// second register before the walk (one extra register move, 0x140 vs 0x13C bytes) and
// colours neg/base/pp/the off induction variable differently (a2/t0/a3/v1 vs v1/a2/t6/t0);
// the loop shape, both return-0 exits, the branch-likely tests and the gate code match.
// 2026-10-28 (wave wX2): the flag updates are compound &=/|= through a byte offset
// (base + idx*4 + 4), which gives the original's base-first address add.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: table-scan flag setter. The slot pointer is &D_0024D7C0[D_00810700][D_00810701];
// the slot holds a list of 0x28-byte records ended by cls 0xFF. Each pass re-reads the
// slot. A record whose cls is not 0xB aborts the scan (returns 0). When cls == 0xB and
// the record key (short at +4) equals arg0, idx = (u16 at +6) >> 8 selects the flags
// word base[idx + 1] in the int table at *(int *)0x70003250. Bit 0x20000000 gates on the
// sign of arg0: act when (arg0 < 0 && bit set) or (arg0 >= 0 && bit clear); otherwise
// the scan continues. On a match: arg1 != 0 clears bit 0x40000000, else sets it;
// returns 1. Reaching the 0xFF end returns 0.
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern int D_0024D7C0[];

typedef struct {
    short cls;
    short pad2;
    short key;
    unsigned short info;
    char rest[0x20];
} Rec;

int func_0019C6F0(int arg0, int arg1) {
    int n;
    int neg;
    Rec *rec;
    Rec * volatile *pp;
    Rec *list;
    int *base;
    int idx;

    n = 0;
    pp = (Rec * volatile *)D_0024D7C0[D_00810700] + D_00810701;
    neg = arg0 & 0x80000000;
    base = *(int **)0x70003250;
    list = *pp;
    for (; ; n++) {
        if (list[n].cls == 0xFF) {
            return 0;
        }
        rec = &(*pp)[n];
        if (rec->cls == 0xB) {
            if (arg0 == rec->key) {
                idx = rec->info >> 8;
                if (neg != 0) {
                    if (base[idx + 1] & 0x20000000) {
                        goto act;
                    }
                } else {
                    if (!(base[idx + 1] & 0x20000000)) {
                        goto act;
                    }
                }
            }
        } else {
            return 0;
        }
    }
act:
    if (arg1 != 0) {
        *(int *)((char *)base + idx * 4 + 4) &= 0xBFFFFFFF;
    } else {
        *(int *)((char *)base + idx * 4 + 4) |= 0x40000000;
    }
    return 1;
}
