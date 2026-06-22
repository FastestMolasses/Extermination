// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// State-advance predicate on an actor (arg0). If the u16 at +0x2E is 0 it bails
// returning 0 (not armed). Otherwise it updates the actor's phase byte at +0x275
// and clears the +0x7 flag, returning 1:
//   - if +0x275 is already nonzero, it just resets it to 0;
//   - else, global D_00810CA4 == 2 forces phase 5;
//   - else it maps global D_00810CA6 (a mode/select byte) to a phase:
//       2->1, 3->2, 1->3, 4->4, and 0 -> bail returning 0 (leaving +0x7 alone).
// Returns 1 on every path that reaches the trailing `+0x7 = 0`.
//
// mwcc 2.3.3 (mwcps2-2.3.3-000906): the dispatch is an if/switch chain whose
// case-compare order (4,1,3,2,0) and shared `v0=1; +7=0` tail only fall out
// when the +0x275!=0 store is the OUT-OF-LINE else (hence `if (+0x275 == 0)`
// with the store-zero in the else) and the inner switch lists cases 0,2,3,1,4.
// 991202 reorders the blocks / fills idiom-13 nops differently (75.2%). 233 ->
// objdiff 100.0 vs build/expected/func_0017AAD0.o.
extern unsigned char D_00810CA4;
extern unsigned char D_00810CA6;

int func_0017AAD0(unsigned char *arg0) {
    if (*(unsigned short *)(arg0 + 0x2E) == 0) {
        return 0;
    }
    if (arg0[0x275] == 0) {
        if (D_00810CA4 == 2) {
            arg0[0x275] = 5;
        } else {
            switch (D_00810CA6) {
            case 0:
                return 0;
            case 2:
                arg0[0x275] = 1;
                break;
            case 3:
                arg0[0x275] = 2;
                break;
            case 1:
                arg0[0x275] = 3;
                break;
            case 4:
                arg0[0x275] = 4;
                break;
            }
        }
    } else {
        arg0[0x275] = 0;
    }
    arg0[7] = 0;
    return 1;
}
