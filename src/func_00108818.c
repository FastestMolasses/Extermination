// NEARMISS func_00108818  (vram 0x00108818, 0x88 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 63.06% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// gcc-2.9 emits movn (conditional move) for the (hi==0xFF00<<24)?0x20:0 sh-selection; target uses explicit branches (beq/bnel/b) and tail-duplicates the dsllv. movn-vs-branch is a deterministic -O2 control-flow-shape codegen choice; permuter diverged (best clean 1545, far worse than 63% base). long sh keeps movn; int ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char D_00241238[];

long func_00108818(unsigned int idx, long arg)
{
    long result = 0;

    if (idx < 0xA) {
        unsigned int off = idx << 4;
        long hi = *(long *)(D_00241238 + off + 8);
        long full = (long)0xFFFF << 24;
        long sh = 0;

        if (hi == full) {
            sh = 0x18;
        } else if ((unsigned long)full < (unsigned long)hi) {
            sh = 0;
        } else if (hi == ((long)0xFF00 << 24)) {
            sh = 0x20;
        }

        result = *(long *)(D_00241238 + off) | (arg << sh);
    }

    return result;
}
