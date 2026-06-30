// NEARMISS func_00109E68  (vram 0x00109E68, 0x124 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 80.71% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc coloring + address-rematerialization wall. Decoded fully (control flow + semantics correct; C compiles to nearly the right shape). Three coupled, non-source-controllable residuals: (1) GPR coloring s1<->s2 swap (expected: pointer->s1, result->s2; ours flips them) -- pure coloring, no permuter lever per s84; (2...
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

    if (D_0024141C == 0) {
        a0 = D_00241404;
        p[2] = a0 - s0[0x2B];
        D_00241404 = a0 + 1;
        s0[1] = s0[1] + 1;
    }

    return s2;
}
