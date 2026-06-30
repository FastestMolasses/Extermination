// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Per-frame slice keyed on a global mode byte and a per-object state byte.
// First: if the global mode D_00810CA4 is 0 or 2, just clears arg0+1; otherwise
// publishes a quadword (copy_qw4) from D_008103D0+0x90 into (*D_00275B40)+0x90.
// Then a 2-way state machine on the byte at arg0+5: state 0 advances the state
// (arg0[5]=st+1) only if the engine flag D_008106CC is set; state 1 tears down
// the actor record D_008102B0 (func_0015C310(...,1)), clears D_008106C6 and
// D_008106CC, and resets arg0[5]=0.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906) at objdiff 100.0; the pinned
// 991202 build reaches 89.6%. Two keys: (1) the state dispatch must be a switch
// (ascending case labels 0,1) so mwcc emits the target's descending beq==1 /
// beqz==0 dispatch with the early-epilogue default; (2) -sdatathreshold 4 puts
// only the 4-byte pointer D_00275B40 in sdata (gp-relative `lw v1,0(gp)`), so the
// far 0x810xxx globals are declared oversized (arrays) to keep them absolute.
extern void copy_qw4(void *dst, void *src);
extern void func_0015C310(char *p, int n);
extern int *D_00275B40;
extern int D_008103D0[16];
extern char D_008102B0[1024];
extern unsigned char D_008106C6[16];
extern unsigned char D_008106CC[16];
extern unsigned char D_00810CA4[16];

void func_00188AC0(char *arg0) {
    unsigned char st;

    if (D_00810CA4[0] == 2 || D_00810CA4[0] == 0) {
        *(char *)(arg0 + 1) = 0;
    } else {
        copy_qw4((void *)(*D_00275B40 + 0x90), (void *)(D_008103D0[0] + 0x90));
    }
    st = *(unsigned char *)(arg0 + 5);
    switch (st) {
    case 0:
        if (D_008106CC[0] != 0) {
            *(unsigned char *)(arg0 + 5) = st + 1;
        }
        break;
    case 1:
        D_008106C6[0] = 0;
        func_0015C310(D_008102B0, 1);
        D_008106CC[0] = 0;
        *(unsigned char *)(arg0 + 5) = 0;
        break;
    }
}
