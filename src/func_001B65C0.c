// NEARMISS func_001B65C0  (vram 0x001B65C0, 0xA0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.03% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register allocation: caller-saved temp held across a call. Body/structure fully match (gp_rel split for D_00275BE4/E8 vs absolute D_0081xxxx achieved via array-decl idiom + threshold 8; full int shift, no andi mask). Sole residual: the input pointer `p` is incremented and reloaded across the call...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

extern unsigned char D_00810788[];
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];
extern unsigned char D_00810B40[];
extern unsigned char *D_00275BE4;
extern int D_00275BE8;

extern void func_001B64F0(int a0, int a1);

void func_001B65C0(int *p) {
    unsigned char *q;
    int m;

    if (D_00810788[0] == 0xFF) {
        D_00275BE4 = D_00810B40;
        D_00275BE4 = D_00275BE4 + D_00810700[0];
        q = D_00275BE4;
        m = 1 << D_00810701[0];
        if ((*q & m) == 0) {
            *q = *q | m;
            do {
                D_00275BE8 = *p;
                func_001B64F0(D_00275BE8, D_00810700[0]);
                p++;
            } while (*p != 0);
        }
    }
}
