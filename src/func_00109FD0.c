// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 78.07% NEARMISS).
// Fixes read from the original instructions:
//  - func_00107590 is declared void, as its definition in src/ is; the int
//    declaration changed which scratch register held D_00241404 afterwards
//    (this was the "coloring wall").
//  - The expected-mode selector (2, or 1 when the channel's 0xD4 field is not 1)
//    and the "second poll succeeded" test are written as "set a default, then
//    overwrite it in an if". The original builds both as conditional moves, not
//    as branches.
//  - The busy flag D_0024141C is cleared right after func_00107590, before the
//    frame counters are updated, not after them.
//
// Clears D_0024141C, computes a flag (a2 == -1 or a1 < a2), and on first use
// (sub-object at a0+0x40 has field 8 == 0) zeroes a0->f8 and sets that field.
// Calls func_00107648(0) and, if it returned nonzero and the flag is set,
// sub_odd_number_of_field_pictures(D_00241404, sub->f4). Sets D_0024141C = 1.
// If func_00106B88() returns 0: calls func_0010A140(a0), sets sub->f0 = 1 and
// returns 0. Otherwise, if D_002414AC equals (sub->fD4 != 1 ? 1 : 2): repeats
// the func_00107648(1) / sub_odd... pair (its nonzero result becomes the return
// value 1), calls func_00107590(D_00241404, sub->f4), clears D_0024141C, sets
// a0->f8 = D_00241404 - sub->fAC, increments D_00241404 and sub->f4, and calls
// func_00109B70(a0) when the flag is clear. A mode mismatch returns -1.

extern int D_00241404;
extern int D_0024141C;
extern int D_002414AC;
extern int func_00107648(int a0);
extern int func_00106B88(void);
extern int func_0010A140(void *a0);
extern void func_00107590(int a0, int a1);
extern int func_00109B70(void *a0);
extern int sub_odd_number_of_field_pictures(int a0, int a1);

struct ch {
    int f0;
    int f4;
    int f8;
    char pad[0xAC - 0xC];
    int fAC;
    char pad2[0xD4 - 0xB0];
    int fD4;
};
struct obj {
    char pad8[0x8];
    int f8;
    char pad40[0x40 - 0xC];
    struct ch *f40;
};

int func_00109FD0(struct obj *a0, int a1, int a2) {
    struct ch *c = a0->f40;
    int flag = 0;
    int want;
    int ok;

    D_0024141C = 0;
    if (a2 == -1 || a1 < a2) {
        flag = 1;
    }
    if (c->f8 == 0) {
        a0->f8 = 0;
        c->f8 = 1;
    }
    if (func_00107648(0) != 0 && flag != 0) {
        sub_odd_number_of_field_pictures(D_00241404, c->f4);
    }
    D_0024141C = 1;
    if (func_00106B88() == 0) {
        func_0010A140(a0);
        c->f0 = 1;
        return 0;
    }
    want = 2;
    if (c->fD4 != 1) want = 1;
    if (D_002414AC == want) {
        int r = 0;
        ok = 0;
        if (func_00107648(1) != 0) ok = 1;
        if (ok && flag) {
            if (sub_odd_number_of_field_pictures(D_00241404, c->f4) != 0) r = 1;
        }
        func_00107590(D_00241404, c->f4);
        D_0024141C = 0;
        a0->f8 = D_00241404 - c->fAC;
        D_00241404 = D_00241404 + 1;
        c->f4 = c->f4 + 1;
        if (flag == 0) {
            func_00109B70(a0);
        }
        return r;
    }
    return -1;
}
