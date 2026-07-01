// NEARMISS func_001FD950  (vram 0x001FD950, 0x22C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.83% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation + instruction-scheduling + branch-likely-sense permutation. Body fully recovered and logically correct (verified against asm). Residuals at 85.83% (mwcc 2.3.3, best build): (1) saved-reg coloring swap on tbl/counter/running-value (target s2/s1/s3, mine s1/s3/s2); (2) v1<->a1 c...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS (mwcc 2.3.3: 85.83%, pinned 991202: 72.63%) — body fully
// recovered; residual is pure compiler-artifact (register coloring +
// instruction scheduling + branch-likely sense), permuter territory.
//
// Radio/subtitle text presenter, called per frame on the message entity
// (arg0). Reads the text-record index from entity+0x34 (low 31 bits, sign-
// extended into a u64 'idx'; the top bit selects table D_0028A4E8 vs
// D_0028A594). Lays out the first TWO '\n' segments via func_001FE530 into
// scratch widths w[0]/w[1] (func_001CC170 = per-glyph pixel width), then
// renders the whole line centered: x = 0x100 - max(w0,w1)/2, y = 0xC2
// (func_001FE070 walks/draws the text). There is NO typewriter; the full
// line draws every frame.
//
// After drawing, the flag-mailbox bookkeeping:
//  - while presenting (+0x6C > 0): if the active flag slot (+0x51) != 0xFF,
//    write D_008106D4[slot]=1 (script handshake), set bit 'slot' in the
//    mask +0x64, and clear +0x51 to 0xFF; then decrement the +0x6C timer
//    and return 0. Slot 0 additionally pings func_001D06E0(player,1) when
//    game-mode spad 0x70003B8F == 2.
//  - at completion (+0x6C <= 0): walk the 12 mask bits; for each set bit,
//    write 2 to D_008106B0[i]+0x24 (completion handshake) and clear the
//    bit; bit 0 additionally pings func_001D06E0(player,0) under spad==2.
//    Returns 1.
//
// gp-rel note (sdatathreshold 8): D_00275838 is a scalar long long -> %gp_rel
// 'ld' (matches the target). The far symbols (D_0028A4E8/D_0028A594/
// D_008102B0/D_008106B0/D_008106D4/D_00820ED0) are declared as sized arrays
// so they stay absolute %hi/%lo as in the target.

extern long long D_00275838;
extern int D_0028A4E8[64];
extern int D_0028A594[64];
extern char D_008102B0[64];
extern char D_008106B0[64];
extern char D_008106D4[64];
extern char D_00820ED0[64];

extern int func_001FE480(int tbl, unsigned long long idx);
extern int func_001FE530(char *p, int v, int z);
extern int func_001CC170(char *p);
extern void func_001FE070(int tbl, unsigned long long idx, int x, int y);
extern void func_001D06E0(char *p, int v);

int func_001FD950(char *arg0) {
    int w[2];
    int tbl;
    int v;
    int i;
    int x;
    unsigned long long idx;
    int field34;
    int *p;
    unsigned char slot;

    *(long long *)w = D_00275838;
    field34 = *(int *)(arg0 + 0x34);
    idx = ((unsigned long long) field34 << 0x21) >> 0x21;
    if (field34 & 0x80000000) {
        tbl = D_0028A4E8[0];
    } else {
        tbl = D_0028A594[0];
    }
    v = func_001FE480(tbl, idx);
    p = w;
    i = 0;
    do {
        v = func_001FE530(D_00820ED0, v, 0);
        i += 1;
        *p++ = func_001CC170(D_00820ED0);
    } while (i < 2);
    if (w[0] < w[1]) {
        x = 0x100 - (w[1] >> 1);
    } else {
        x = 0x100 - (w[0] >> 1);
    }
    func_001FE070(tbl, idx, x, 0xC2);
    if (*(int *)(arg0 + 0x6C) > 0) {
        slot = *(unsigned char *)(arg0 + 0x51);
        if (slot != 0xFF) {
            if ((*(unsigned char *)0x70003B8F == 2) && (slot == 0)) {
                func_001D06E0(D_008102B0, 1);
            }
            *(unsigned char *)(D_008106D4 + *(unsigned char *)(arg0 + 0x51)) = 1;
            *(int *)(arg0 + 0x64) |= 1 << *(unsigned char *)(arg0 + 0x51);
            *(unsigned char *)(arg0 + 0x51) = 0xFF;
        }
        *(int *)(arg0 + 0x6C) -= 1;
        return 0;
    }
    {
        char *rec = D_008106B0;
        int bit = 0;
        do {
            if (*(int *)(arg0 + 0x64) & (1 << bit)) {
                if (*(unsigned char *)0x70003B8F == 2) {
                    if (bit == 0) {
                        func_001D06E0(D_008102B0, 0);
                    }
                }
                *(char *)(rec + 0x24) = 2;
                *(int *)(arg0 + 0x64) &= ~(1 << bit);
            }
            bit += 1;
            rec += 1;
        } while (bit < 0xC);
    }
    return 1;
}
