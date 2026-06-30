// NEARMISS func_001FE9E0  (vram 0x001FE9E0, 0x88 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 78.53% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Constant-rematerialization + branch-sense permutation: target re-emits li v0,1 inside each D_00275C58-store block and uses bgez (>=0 -> reset) sense; mwcc places li 1 once at merge and canonicalizes the countdown test to bltz. Body/structure fully correct; permuter territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p

//
// State-machine step on the global phase var D_00275C5C (sibling of
// func_001FECB0; here the dispatch values are 0 = init, 2 = running/poll).
// Returns a status in var_s0 (-1 by default).
// State 0: arms the phase (D_00275C5C = 2) and kicks func_00114360(); if that
// returns nonzero, flags D_00275C58 = 1; returns -1 either way.
// State 2: reads the countdown D_00275C64 -- if negative, just flags
// D_00275C58 = 1 and returns -1; if >= 0, resets the phase (D_00275C5C = 0) and
// returns D_00275C64 (without flagging). The default/unknown phase flags
// D_00275C58 = 1 and returns -1.
//
// NEARMISS 78.5% (both mwcc 2.3.3 and 991202). Body/structure fully recovered;
// the residuals are a constant-rematerialization + branch-sense permutation:
// the target re-emits `li v0,1` inside each D_00275C58-store block and uses a
// `bgez` (>=0 -> reset) sense, while mwcc places the `li 1` once at a merge and
// canonicalizes the countdown test to `bltz`. Regalloc/scheduling wall --
// permuter territory.

extern int func_00114360(void);
extern int D_00275C58;
extern int D_00275C5C;
extern int D_00275C64;

int func_001FE9E0(void) {
    int var_s0;
    int var_v0;

    var_s0 = -1;
    switch (D_00275C5C) {
    case 0:
        D_00275C5C = 2;
        var_v0 = -1;
        if (func_00114360() != 0) {
        default:
block_9:
            D_00275C58 = 1;
block_10:
            var_v0 = var_s0;
        }
        break;
    case 2:
        if (D_00275C64 < 0) {
            goto block_9;
        }
        D_00275C5C = 0;
        var_s0 = D_00275C64;
        goto block_10;
    }
    return var_v0;
}
