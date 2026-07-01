// NEARMISS func_002280F0  (vram 0x002280F0, 0x22C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.14% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation + branch-likely-sense + arg-emit-order permutation across the state machine. CW emits li a0,1 first, colors case-1 literal into t0 (mwcc v0), emits the p+0x20 counter cap as a branch-likely bnezl with the increment in the delay slot (mwcc uses plain bnez), and uses movz vs mwc...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_001FE9A0(int a0, int a1, int a2);
extern int func_001FECB0(int a0, int a1, void *a2, void *a3, void *t0);
extern int func_001FEC20(int a0, int a1, void *a2, int a3, void *t1);
extern void func_001FE8D0(void);
extern int func_00228320(unsigned char *p);
extern unsigned char D_002738E0;
extern int D_00275C58;
extern unsigned char D_00821500;

int func_002280F0(unsigned char *p) {
    int r;
    int s;
    int a;

    if (func_001FE9A0(1, (int)(p + 0x50), (int)(p + 0x54)) == 0) {
        return 0;
    }
    switch (*(unsigned char *)(p + 2)) {
    case 0:
        r = func_001FECB0(*(int *)(p + 0x48), *(int *)(p + 0x4C), p + 0x58, 0, p + 0x60);
        if (r == 0) {
            s = *(int *)(p + 0x58);
            if (s != 2) {
                return 4;
            }
            if (s == 2) {
                if (*(int *)(p + 0x60) == 1) {
                    *(unsigned char *)(p + 2) = 1;
                    break;
                }
                a = *(int *)(p + 0x20);
                if (a >= 6) {
                    *(int *)(p + 0x20) = 0;
                    func_001FE8D0();
                    return 2;
                }
                *(int *)(p + 0x20) = a + 1;
            }
            break;
        }
        if (r == -1) {
            break;
        }
        return 4;
    case 1:
        r = func_001FEC20(*(int *)(p + 0x48), *(int *)(p + 0x4C), &D_002738E0, 0, &D_00821500);
        if (r < 0) {
            break;
        }
        if (r != 0) {
            return 1;
        }
        if (*(unsigned char *)(p + 0x14) == 2) {
            return 1;
        }
        *(unsigned char *)(p + 2) = 2;
        break;
    case 2:
        r = func_001FECB0(*(int *)(p + 0x48), *(int *)(p + 0x4C), p + 0x58, p + 0x5C, p + 0x60);
        if (r == 0) {
            s = *(int *)(p + 0x58);
            if (s != 2) {
                return 4;
            }
            if (s == 2) {
                if (*(int *)(p + 0x60) == 1) {
                    return func_00228320(p) == 0 ? 3 : 1;
                }
                a = *(int *)(p + 0x20);
                if (a >= 6) {
                    *(int *)(p + 0x20) = 0;
                    func_001FE8D0();
                    return 2;
                }
                *(int *)(p + 0x20) = a + 1;
            }
            break;
        }
        if (r == -1) {
            break;
        }
        return 4;
    }
    return D_00275C58 != 0 ? 4 : 0;
}
