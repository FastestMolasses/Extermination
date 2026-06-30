// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Per-state dispatcher on the state byte at offset 9 of the global object
// pointed to by *(void**)0x70003B6C. Reads st = obj[9]; q = &obj[9].
//  state 0: calls func_001FBC50(st, q) then func_001FABB0(), reloads the global
//           object pointer, and bumps obj[9] (advance to next state).
//  state 1: if the gate byte D_00282157 == 0, sets D_00821058 = 1, clears the
//           gp-rel flag D_00275C78, and advances *q (obj[9]).
//  state 2: advances *q (obj[9]).
//  state 3: returns 2 (sequence complete); all other states return 0.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); pinned 991202 reaches 92.65%.
// Keys: (1) plain `switch(st)` reproduces the target's 3->2->1->0 descending
// compare chain with the case-0 beqz and the shared `paddub v0,zero,zero`
// zero-result block that cases 0/1/2 funnel into; (2) -sdatathreshold 4 keeps
// D_00275C78 gp-relative while the far .data globals D_00282157 / D_00821058 are
// over-declared as arrays so they exceed the threshold and get absolute %hi/%lo
// addressing (idiom #20). objdiff 100.0 vs expected.
extern void func_001FBC50(unsigned char st, unsigned char *p);
extern void func_001FABB0(void);
extern signed char D_00282157[16];
extern signed char D_00821058[16];
extern signed char D_00275C78;

int func_001AC3B0(void) {
    unsigned char *p;
    unsigned char st;
    unsigned char *q;

    p = *(unsigned char **)0x70003B6C;
    st = p[9];
    q = p + 9;
    switch (st) {
    case 0:
        func_001FBC50(st, q);
        func_001FABB0();
        p = *(unsigned char **)0x70003B6C;
        p[9] = p[9] + 1;
        break;
    case 1:
        if (D_00282157[0] == 0) {
            D_00821058[0] = 1;
            D_00275C78 = 0;
            *q = *q + 1;
        }
        break;
    case 2:
        *q = *q + 1;
        break;
    case 3:
        return 2;
    }
    return 0;
}
