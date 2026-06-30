// NEARMISS func_001FECB0  (vram 0x001FECB0, 0x84 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.70% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Regalloc/constant-rematerialization permutation: target keeps compare constant 1 live in v0 and reuses it for D_00275C58 stores (mwcc re-emits li v0,1 at merge tail); colors D_00275C64 reload as v1 (mwcc picks v0). Body/structure fully correct; permuter territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p

//
// State-machine step on the global phase var D_00275C5C (a 3-way dispatch:
// 0 = init, 1 = running/poll). Returns a status in var_s0 (-1 by default).
// State 0: arms the phase (D_00275C5C = 1) and kicks func_00114988(); if that
// returns nonzero, flags D_00275C58 = 1; returns -1 either way.
// State 1: reads the countdown D_00275C64 -- if nonzero, returns D_00275C64-1
// and flags D_00275C58 = 1; if zero, resets the phase (D_00275C5C = 0) and
// returns 0 (without flagging). The default/unknown phase just flags
// D_00275C58 = 1 and returns -1.
//
// NEARMISS 84.7% (mwcc 2.3.3; 991202 = 84.5%). Body/structure fully recovered;
// the sole residuals are a register-coloring + constant-rematerialization
// permutation: the target keeps the compare constant 1 live in $v0 and reuses
// it for the D_00275C58 stores (mwcc re-emits `li v0,1` at the merge tail), and
// colors the D_00275C64 reload as $v1 (mwcc picks $v0). Regalloc/scheduling wall
// -- permuter territory.

extern int func_00114988(void);
extern int D_00275C58;
extern int D_00275C5C;
extern int D_00275C64;

int func_001FECB0(void) {
    int var_s0;
    int var_v0;

    var_s0 = -1;
    switch (D_00275C5C) {
    case 0:
        D_00275C5C = 1;
        var_v0 = -1;
        if (func_00114988() != 0) {
        default:
block_8:
            D_00275C58 = 1;
block_9:
            var_v0 = var_s0;
        }
        break;
    case 1:
        if (D_00275C64 != 0) {
            var_s0 = D_00275C64 - 1;
            goto block_8;
        }
        D_00275C5C = 0;
        var_s0 = 0;
        goto block_9;
    }
    return var_v0;
}
