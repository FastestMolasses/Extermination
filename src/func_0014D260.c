// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-frame tick for a sub-actor whose record begins at arg0+0x1F0. Gated off a
// global pause/freeze flag D_00810803 (==1 -> do nothing). Dispatches on the
// sub-mode byte at arg0+4 to one of four state handlers (case 3 = E640 returns
// immediately; cases 0/1/2 = D3B0/D470/E370 fall through to the common timer
// decrement). The handlers take (arg0, record). The common tail counts down six
// timer fields in the record (bytes 0x5A/0x5B/0x5E and shorts 0x56/0x52/0x50,
// each clamped at 0) then calls func_001F91C0(arg0).
//
// Matched with mwcc 2.3.3: the switch lowers to a beql/beq case-chain tested in
// descending case order with the case-1 compare reusing the a1=1 constant from
// the D_00810803 guard -- the 991202 build mis-schedules the chain (93.83%).
extern void func_0014D3B0(char *, char *);
extern void func_0014D470(char *, char *);
extern void func_0014E370(char *, char *);
extern void func_0014E640(char *, char *);
extern void func_001F91C0(char *);
extern unsigned char D_00810803;

void func_0014D260(char *arg0) {
    char *p;
    int v;
    int h;

    p = arg0 + 0x1F0;
    if (D_00810803 == 1) {
        return;
    }
    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        func_0014D3B0(arg0, p);
        break;
    case 1:
        func_0014D470(arg0, p);
        break;
    case 2:
        func_0014E370(arg0, p);
        break;
    case 3:
        func_0014E640(arg0, p);
        return;
    }
    v = *(char *)(p + 0x5A);
    if (v != 0) {
        *(char *)(p + 0x5A) = v - 1;
    }
    h = *(short *)(p + 0x56);
    if (h != 0) {
        *(short *)(p + 0x56) = h - 1;
    }
    v = *(char *)(p + 0x5B);
    if (v != 0) {
        *(char *)(p + 0x5B) = v - 1;
    }
    v = *(char *)(p + 0x5E);
    if (v != 0) {
        *(char *)(p + 0x5E) = v - 1;
    }
    h = *(short *)(p + 0x52);
    if (h != 0) {
        *(short *)(p + 0x52) = h - 1;
    }
    h = *(short *)(p + 0x50);
    if (h != 0) {
        *(short *)(p + 0x50) = h - 1;
    }
    func_001F91C0(arg0);
}
