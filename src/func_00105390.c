// NEARMISS func_00105390  (vram 0x00105390, 0x188 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.39% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc branch-likely-sense wall: body byte-identical at 99.39%; sole residual is one annul branch where expected emits `beql v1,$zero` (beqzl) and ee-gcc 2.9 emits the opposite-sense `bnezl` for the D_002414B0==0 test (both branch-likely, identical bodies). Inverting the C condition flips gcc to a NON-likely `beqz` A...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_002414AC;
extern int D_002414B0;

void func_00105390(int *out, int *in, int a2, int a3)
{
    if (D_002414AC == 3) {
        if (D_002414B0 == 0) {
            out[0] = ((a2 > 0 ? a2 + 1 : a2) >> 1) + in[0];
            out[1] = ((a3 > 0 ? a3 + 1 : a3) >> 1) + in[1] - 1;
            out[2] = (((a2 * 3) + (a2 > 0 ? 1 : 0)) >> 1) + in[0];
            out[3] = (((a3 * 3) + (a3 > 0 ? 1 : 0)) >> 1) + in[1] + 1;
        } else {
            out[0] = (((a2 * 3) + (a2 > 0 ? 1 : 0)) >> 1) + in[0];
            out[1] = (((a3 * 3) + (a3 > 0 ? 1 : 0)) >> 1) + in[1] - 1;
            out[2] = ((a2 > 0 ? a2 + 1 : a2) >> 1) + in[0];
            out[3] = ((a3 > 0 ? a3 + 1 : a3) >> 1) + in[1] + 1;
        }
    } else {
        int v;
        out[0] = ((a2 > 0 ? a2 + 1 : a2) >> 1) + in[0];
        v = ((a3 > 0 ? a3 + 1 : a3) >> 1) + in[1];
        out[1] = v;
        if (D_002414AC == 1) {
            out[1] = v - 1;
        } else {
            out[1] = v + 1;
        }
    }
}
