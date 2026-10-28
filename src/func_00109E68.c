// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 91.07% NEARMISS).
// Fixes read from the original instructions:
//  - func_00107590 is declared void and func_00109B70 takes one int, as their
//    definitions in src/ are.
//  - The first arm's result is built in its own variable (zero, then the
//    "sub_odd... returned nonzero" test when func_00107648(0) succeeded) and copied
//    into the result at the end of the arm; the poll result is read into a local
//    before that variable is cleared. This keeps the original's temporary and
//    saved-register colouring (pointer arg in the second saved register).
//  - In the "a1 reached a2" arm the skip flag is set after func_00109B70, not
//    before the poll; that placement gives the original's call delay slot and
//    the high-half placement of D_00241404's address.
//
// Frame-advance step for the picture channel at a0->f40. If a2 == -1 or a1 < a2:
// on first use (channel f8 == 0) clears a0->f8 and sets channel f8 = 1, then polls
// func_00107648(0) and, on success, returns whether
// sub_odd_number_of_field_pictures(D_00241404, channel f4) is nonzero. Otherwise
// it returns func_00107648(0)'s result, calls func_00109B70(a0) and skips the
// toggle below. Then calls func_00107590(D_00241404, channel f4); unless skipped or
// D_002414AC is 3 (frame picture), toggles the pending-field flag D_0024141C.
// Stores a0->f8 = D_00241404 - channel fAC and, when D_0024141C is clear,
// increments D_00241404 and channel f4.

extern int D_00241404;
extern int D_0024141C;
extern int D_002414AC;
extern int func_00107648(int a0);
extern int sub_odd_number_of_field_pictures(int a0, int a1);
extern void func_00107590(int a0, int a1);
extern int func_00109B70(int a0);

struct ch {
    int f0;
    int f4;
    int f8;
    char pad[0xAC - 0xC];
    int fAC;
};
struct obj {
    char pad8[0x8];
    int f8;
    char pad40[0x40 - 0xC];
    struct ch *f40;
};

int func_00109E68(struct obj *a0, int a1, int a2) {
    struct ch *c = a0->f40;
    int flag = 0;
    int r;

    if (a2 == -1 || a1 < a2) {
        int t;
        int ok;
        if (c->f8 == 0) {
            a0->f8 = 0;
            c->f8 = 1;
        }
        ok = func_00107648(0);
        t = 0;
        if (ok != 0) {
            t = (sub_odd_number_of_field_pictures(D_00241404, c->f4) != 0);
        }
        r = t;
    } else {
        r = func_00107648(0);
        func_00109B70((int)a0);
        flag = 1;
    }

    func_00107590(D_00241404, c->f4);
    if (D_002414AC != 3 && !flag) {
        D_0024141C = (D_0024141C == 0);
    }
    a0->f8 = D_00241404 - c->fAC;
    if (D_0024141C == 0) {
        D_00241404 = D_00241404 + 1;
        c->f4 = c->f4 + 1;
    }
    return r;
}
