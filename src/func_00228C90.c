// NEARMISS func_00228C90  (vram 0x00228C90, 0x220 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.40% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Pure register-allocation permutation near-miss. Logic fully recovered and correct (all instructions present, only operand registers differ). The target colors arg0=s0, found_base=s2, loop-pointer=s1, loop-counter=s3; mwcc gives me arg0=s3, found_base=s0, loop-pointer=s2, loop-counter=s1. The swap...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 86.4% (mwcc 2.3.3; 82.9% on 991202). Logic fully recovered; the sole
// residual is a register-allocation permutation: the target colors arg0=s0,
// found_base=s2, loop-pointer=s1, loop-counter=s3, while mwcc gives arg0=s3,
// found_base=s0, pointer=s2, counter=s1. Every instruction matches; only operand
// registers differ, cascaded across the four scan loops. Register-order class.
//
// Scans an array of arg0 records (base D_00821500, stride 0x40, name field at +0x20)
// and sets 8 "found" flags by matching each record name against a set of expected
// names via func_00123020 (== 0 means match): flag0=D_00266F00, flag1=D_00275890,
// flag2=D_00273970, then a combined pass sets flag3..7 from D_00266F20/F60/FA0/FE0
// and D_00267020. Returns 1 only if all 8 flags were found, else 0.
extern int func_00123020(int a0, char *s);
extern char D_00821500[];
extern char D_00266F00[];
extern char D_00266F20[];
extern char D_00266F60[];
extern char D_00266FA0[];
extern char D_00266FE0[];
extern char D_00267020[];
extern char D_00273970[];
extern char D_00275890[];

int func_00228C90(int arg0) {
    int found[8];
    int i;
    int n;
    char *p;

    for (i = 0; i < 8; i++) {
        found[i] = 0;
    }

    n = 0;
    p = D_00821500;
    while (n < arg0) {
        if (func_00123020((int)(p + 0x20), D_00266F00) == 0) {
            found[0] = 1;
        }
        n++;
        p += 0x40;
    }

    n = 0;
    p = D_00821500;
    while (n < arg0) {
        if (func_00123020((int)(p + 0x20), D_00275890) == 0) {
            found[1] = 1;
        }
        n++;
        p += 0x40;
    }

    n = 0;
    p = D_00821500;
    while (n < arg0) {
        if (func_00123020((int)(p + 0x20), D_00273970) == 0) {
            found[2] = 1;
        }
        n++;
        p += 0x40;
    }

    n = 0;
    p = D_00821500;
    while (n < arg0) {
        if (func_00123020((int)(p + 0x20), D_00266F20) == 0) {
            found[3] = 1;
        }
        if (func_00123020((int)(p + 0x20), D_00266F60) == 0) {
            found[4] = 1;
        }
        if (func_00123020((int)(p + 0x20), D_00266FA0) == 0) {
            found[5] = 1;
        }
        if (func_00123020((int)(p + 0x20), D_00266FE0) == 0) {
            found[6] = 1;
        }
        if (func_00123020((int)(p + 0x20), D_00267020) == 0) {
            found[7] = 1;
        }
        n++;
        p += 0x40;
    }

    for (i = 0; i < 8; i++) {
        if (found[i] == 0) {
            return 0;
        }
    }
    return 1;
}
