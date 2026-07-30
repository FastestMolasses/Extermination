// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS:
//   Request pump for a small global state machine, dispatched on req[2] (+0x8)
//   through jtbl_0026DFA0 (7 entries; any other value returns 0).  state[4] is
//   the per-caller handshake byte (0 = not yet issued, 1 = issued).
//   D_002821B0 = request kind latched for the worker, D_002821B4 = phase
//   (1 = pending, 2 = done), D_002821B8/BC = the two payload words copied from
//   req[5]/req[6] (+0x14/+0x18), D_00282224 = 16-bit status word whose bit 15
//   means "ready", D_00282240 = auxiliary mode word.  Returns 1 when the
//   request has completed (or was accepted outright), 0 while still pending.
//   $a0 is never read by the original code; the unused first parameter is
//   required so that `state` and `req` land in $a1/$a2.
//
//   D_002821B0 and D_002821B4 are declared volatile so the post-RA list
//   scheduler keeps the "kind then phase" store order: otherwise mwcc always
//   issues the D_002821B4 store first (its value is already live in a reg)
//   ahead of the D_002821B0 / D_00282240 store whose constant still needs an
//   addiu.  Volatile is also the honest description of this handshake block.
//

extern volatile int D_002821B0;
extern volatile int D_002821B4;
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
