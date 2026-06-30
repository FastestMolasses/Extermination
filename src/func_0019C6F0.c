// NEARMISS func_0019C6F0  (vram 0x0019C6F0, 0x140 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 72.38% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-allocation/coloring permutation (identical instruction sequence and operands, only register names differ: v0<->v1, a3<->t1, t4<->t5 throughout) plus bnel-vs-beq branch-likely sense on the cls!=0xB guard and one redundant cur=*pp move. NOT the clean-store delay-slot nop (mwcc233 scores lo...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS (m233_5 decode): table-scan flag setter. desc = D_0024D7C0[D_00810700];
// base record list = *(desc[D_00810701]); for each 0x28-byte record until cls==0xFF:
// if cls(*(short)rec+0) != 0xB the scan ABORTS (returns 0). When cls==0xB and
// rec[2]==arg0 (the lookup key, lh rec+4), index = (u16 rec+6 >> 8) selects an entry
// in the table at *(int*)0x70003250 (4-int stride). The entry's flags word (+4) carries
// bit 0x20000000 as a gate keyed on the sign of arg0: act when (arg0<0 && bit set) or
// (arg0>=0 && bit clear); otherwise the record is skipped and the scan continues.
// On a matched+gated entry: if arg1!=0 clear bit 0x40000000, else set it; return 1.
// Reaching cls==0xFF (end) or cls!=0xB returns 0.
// NEARMISS: body + control flow fully recovered and verified; sole residual is
// register-allocation/coloring permutation (v0<->v1, a3<->t1, t4<->t5 swaps throughout,
// identical instruction sequence/operands) plus the bnel-vs-beq branch-likely sense on
// the cls!=0xB guard and one redundant cur=*pp move. NOT the clean-store delay-slot nop
// (2.3.3 scores LOWER, 72.7%); this is permuter territory. Best 76.7% under 991202.
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern int D_0024D7C0[];

int func_0019C6F0(int arg0, int arg1) {
    int *table;
    int *pp;
    int *base;
    short *cur;
    short *rec;
    int neg;
    int n;
    int off;
    int idx;

    table = (int *)D_0024D7C0[D_00810700];
    pp = table + D_00810701;
    neg = arg0 & 0x80000000;
    base = *(int **)0x70003250;
    cur = (short *)*pp;
    n = 0;
    off = 0;
    for (;;) {
        if (*cur == 0xFF) {
            return 0;
        }
        rec = (short *)(*pp + off);
        if (*rec != 0xB) {
            return 0;
        }
        if (arg0 == rec[2]) {
            idx = (*(unsigned short *)(rec + 3)) >> 8;
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
        cur += 0x14;
        off += 0x28;
        n += 1;
    }
act:
    if (arg1 != 0) {
        base[idx + 1] = base[idx + 1] & 0xBFFFFFFF;
    } else {
        base[idx + 1] = base[idx + 1] | 0x40000000;
    }
    return 1;
}
