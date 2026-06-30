// NEARMISS func_00109FD0  (vram 0x00109FD0, 0x170 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 78.07% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc coloring/reg-alloc wall (no permuter lever, confirmed s84). Full control flow + branch shapes reproduced byte-identically (incl. forward beqzl/bnezl annul via `a2==-1 || a1<a2` and ?1:0 idioms; matches expected ops 0x34-0x60 and 0x90). Sole residual is GPR coloring: expected holds the &D_0024141C base in s5 (p...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241404;
extern int D_0024141C;
extern int D_002414AC;
extern int func_00107648(int a0);
extern int func_00106B88(void);
extern int func_0010A140(void *a0);
extern int func_00107590(int a0, int a1);
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
    if (D_002414AC == (c->fD4 != 1 ? 1 : 2)) {
        int r = 0;
        if (func_00107648(1) != 0) {
            if (flag != 0) {
                r = sub_odd_number_of_field_pictures(D_00241404, c->f4) ? 1 : 0;
            }
        }
        func_00107590(D_00241404, c->f4);
        a0->f8 = D_00241404 - c->fAC;
        D_00241404 = D_00241404 + 1;
        c->f4 = c->f4 + 1;
        D_0024141C = 0;
        if (flag == 0) {
            func_00109B70(a0);
        }
        return r;
    }
    return -1;
}
