// NEARMISS func_001B7D60  (vram 0x001B7D60, 0x228 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.24% via mwcc 2.3 (mwcps2-2.3-991202) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// jr-table external-dispatch wall (proven s84): lui/addiu %hi/%lo(jtbl_0026DFA0) becomes a local @30 table (reloc mismatch) plus the addiu/sll dispatch reorder. Second residual (3 sites, cases 0/1/4): mwcc's post-RA list scheduler ALWAYS issues the store whose value is already live in $v1 (D_002821...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS: jr-table external-dispatch wall (proven s84). The 7-entry
// dispatch table lives in the consolidated external rodata TU as
// jtbl_0026DFA0; mwcc must emit its own local @NN table, so the lw's reloc
// can never match. Residual beyond that: mwcc's post-RA list scheduler
// always issues the D_002821B4 store (whose value is already live in $v1)
// ahead of the D_002821B0 / D_00282240 store whose constant still needs an
// addiu -- source-order swaps and const-materialization locals were both
// tried and produce byte-identical output, so this is a scheduler wall.
//
// SEMANTICS:
//   Request pump for a small global state machine. req[2] (+0x8) selects the
//   request kind, state[4] is the per-caller handshake byte (0 = not yet
//   issued, 1 = issued). D_002821B0 = request kind latched for the worker,
//   D_002821B4 = phase (1 = pending, 2 = done), D_002821B8/BC = the two
//   payload words copied from req[5]/req[6] (+0x14/+0x18), D_00282224 =
//   16-bit status word whose bit 15 means "ready", D_00282240 = auxiliary
//   mode word. Returns 1 when the request has completed (or was accepted
//   outright), 0 while it is still pending. $a0 is never read by the
//   original code; the unused first parameter is required so that `state`
//   and `req` land in $a1/$a2.

extern int D_002821B0;
extern int D_002821B4;
extern int D_002821B8;
extern int D_002821BC;
extern unsigned short D_00282224;
extern int D_00282240;

extern int func_001FC9B0();

int func_001B7D60(int arg0, unsigned char *state, int *req)
{
    switch (req[2]) {
    case 0:
        switch (state[4]) {
        case 0:
            D_002821B0 = 2;
            D_002821B4 = 1;
            D_002821B8 = req[5];
            D_002821BC = req[6];
            state[4] = 1;
            /* fallthrough */
        case 1:
            if (D_002821B4 == 2) {
                return 1;
            }
        }
        break;
    case 1:
        switch (state[4]) {
        case 0:
            D_002821B0 = 2;
            D_002821B4 = 1;
            D_002821B8 = req[5];
            D_002821BC = req[6];
            if (req[7] != 0) {
                D_00282224 = 1;
                state[4] = 1;
                return 0;
            }
            return 1;
        case 1:
            if (D_00282224 & 0x8000) {
                return 1;
            }
            return 0;
        }
        break;
    case 2:
        if (D_002821B4 != 2 && D_002821B0 != 0) {
            break;
        }
        return 1;
    case 3:
        func_001FC9B0();
        return 1;
    case 4:
        switch (state[4]) {
        case 0:
            D_00282240 = 5;
            D_002821B0 = 4;
            D_002821B4 = 1;
            D_002821B8 = req[5];
            D_002821BC = req[6];
            state[4] = 1;
            /* fallthrough */
        case 1:
            if (D_002821B4 == 2) {
                return 1;
            }
        }
        break;
    case 5:
        D_00282240 = 5;
        D_002821B0 = 4;
        D_002821B4 = 1;
        D_002821B8 = req[5];
        D_002821BC = req[6];
        return 1;
    case 6:
        D_002821B4 = 2;
        return 1;
    }
    return 0;
}
