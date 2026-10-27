// NEARMISS func_00109E68  (vram 0x00109E68, 0x124 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.07% via ee-gcc 2.9-991111-01 (-O2). Matching wave 2026-10-27 corrected the body:
// the p[2] = D_00241404 - s0[0x2B] store runs on EVERY path (the original stores it in a
// shared delay slot before testing D_0024141C); only the counter bumps are conditional.
// Remaining differences: (1) s1/s2 coloring swap of p and the result (the `&&` spelling
// fixes it, 86.71%, but loses the arm shape below); (2) the original computes the first
// arm's result in a call-clobbered temporary copied into the result register at the end of
// that arm, with the &D_00241404 high half materialised separately in each predecessor of
// the join (PRE-style placement); ours assigns the result register directly and hoists the
// high half. Tried: && / ?: / if-else / nested-temp / unsigned / statement-order spellings.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_00107648(int);
extern int sub_odd_number_of_field_pictures(int, int);
extern int func_00109B70(void *);
extern int func_00107590(int, int);
extern int D_00241404;
extern int D_002414AC;
extern int D_0024141C;

int func_00109E68(int *p, int a1, int a2)
{
    int s4 = 0;
    int s2;
    int a0;
    int *s0 = (int *)p[0x10];

    if (a2 == -1 || a1 < a2) {
        if (s0[2] == 0) {
            p[2] = 0;
            s0[2] = 1;
        }
        if (func_00107648(0)) {
            s2 = (sub_odd_number_of_field_pictures(D_00241404, s0[1]) != 0);
        } else {
            s2 = 0;
        }
    } else {
        s4 = 1;
        s2 = func_00107648(0);
        func_00109B70(p);
    }

    func_00107590(D_00241404, s0[1]);

    if (D_002414AC != 3 && !s4) {
        D_0024141C = (D_0024141C == 0);
    }

    a0 = D_00241404;
    p[2] = a0 - s0[0x2B];
    if (D_0024141C == 0) {
        D_00241404 = a0 + 1;
        s0[1] = s0[1] + 1;
    }

    return s2;
}
