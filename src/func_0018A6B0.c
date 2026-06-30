// NEARMISS func_0018A6B0  (vram 0x0018A6B0, 0x1CC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.20% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring + constant-sharing + tail scheduling (991202==233, 87.2%). Body, the s1=arg0/s0=&D_008102B0 split and frame 0x30 all correct. Target reloads constant 1 with fresh 'li v1,1' per store/compare; mwcc CSEs 1 into a2 and reuses it (sb a2 / beq a1,a2). Trailing func-ptr-call guard and...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 87.2% (991202 == 233, so 2.3.3 gives no edge). Body+structure
// fully recovered, including the s1=arg0 / s0=&D_008102B0 register split and
// frame size 0x30 (cached the D_008102B0 base in a local 'g' to force the
// callee-saved-reg coloring that matches). Residuals are register-coloring /
// constant-sharing / tail-block scheduling artifacts: (1) target reloads the
// constant 1 with a fresh 'li v1,1' for each *(arg0+1)/*(arg0+4)=1 store and
// for the case-1 dispatch compare, while mwcc CSEs the 1 into a2 and reuses
// it ('sb a2,..' / 'beq a1,a2'); (2) the trailing func-ptr-call guard block
// and the case-3 func_001AFC10 epilogue are scheduled in a different order.
// Player/entity state tick: outer switch on state byte +4 (0=spawn-probe via
// func_0018A8D0 then ->1; 2=idle; 3=func_001AFC10; 1=main). State 1: set
// +1 flag from globals D_008102B0[+1]/[+0x1F0]==0x33, dispatch sub-state +3
// (0/1/2/4 -> func_00188630/A50/DF0 or 0x1F0 cancel path / func_0018A1F0),
// then if a global/level guard passes and +1 flag set, indirect-call +0x4C.
extern void func_00188630(char *, unsigned char);
extern void func_00188A50(char *, unsigned char);
extern void func_00188DF0(char *, unsigned char);
extern void func_0018A1F0(char *, unsigned char);
extern int func_0018A8D0(void);
extern void func_001AFC10(unsigned char);
extern char D_008102B0;
extern unsigned char D_008106C6;
extern unsigned char D_008106C7;
extern unsigned char D_008106CC;
extern unsigned char D_00810CA6;

void func_0018A6B0(char *arg0) {
    unsigned char st;
    unsigned char sub;
    char *g;

    g = &D_008102B0;
    st = *(unsigned char *)(arg0 + 4);
    switch (st) {
    case 0:
        if (func_0018A8D0() == 0) {
            *(unsigned char *)(arg0 + 4) = 1;
            return;
        }
        /* fallthrough */
    case 2:
        return;
    case 1:
        if (*(unsigned char *)(g + 1) != 0) {
            *(unsigned char *)(arg0 + 1) = 1;
        } else if (*(unsigned char *)(g + 0x1F0) == 0x33) {
            *(unsigned char *)(arg0 + 1) = 1;
        }
        sub = *(unsigned char *)(arg0 + 3);
        switch (sub) {
        case 0:
            func_00188630(arg0, st);
            break;
        case 1:
            func_00188A50(arg0, st);
            break;
        case 2:
            if (D_008106CC != 0) {
                if (*(unsigned char *)(arg0 + 0xD) == 0) {
                    if (D_00810CA6 != 0 && D_008106C7 != 0) {
                        D_008106C7 = 0;
                    }
                }
                *(unsigned char *)(arg0 + 4) = 3;
            } else {
                func_00188DF0(arg0, st);
            }
            break;
        case 4:
            func_0018A1F0(arg0, st);
            break;
        }
        if ((D_008106C6 == 0 || *(int *)(g + 0x230) != 0xC || *(unsigned char *)(g + 0x1F1) != 1) && *(unsigned char *)(arg0 + 1) != 0) {
            (*(void (**)(char *))(arg0 + 0x4C))(arg0);
            return;
        }
        break;
    case 3:
        func_001AFC10(st);
        break;
    }
}
