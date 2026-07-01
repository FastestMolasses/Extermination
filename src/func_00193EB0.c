// NEARMISS func_00193EB0  (vram 0x00193EB0, 0x390 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.26% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// see above
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Actor "hit reaction" event dispatcher. arg1+0x230 is an incoming event/state code; arg0 is the
// actor record (arg0+6 = reaction id, arg0+1 = a busy/lock flag cleared on every dispatch).
// Most codes (0x10, 0x2A/0xD, 0xC/0x29, 0x28, 0x12) just set a fixed reaction id and clear the
// lock flag. Codes {1, 0x21} instead forward to func_00191000(arg0, arg1) when arg2 != 2 and the
// actor isn't already busy/reacting (arg0+5 == 0 && arg0+6 == 0). Codes {8,9,7,6,0x2D,0x2C} are a
// region/area-gated combat trigger: for area 0x16 -> reaction 0xC; for area 0x13 (only when
// D_00810701 == 0 and the two hard-addressed gates D_008106B8/*0x70003B8D are clear) it tests the
// actor's world position bands (arg1+0xA0/0xA4/0xA8) against fixed thresholds and fires
// func_001B0C60(0x13, 0, K) with K in {0xD, 0xB, 0xC} for the matching band, setting reaction 7;
// for area 0xD (same gates) it checks D_00810702 in {4,6} or {5,7} against a Z threshold (159.0f)
// and fires func_001B0C60(0x13, 0, K) with K in {9, 0xA}, also setting reaction 7.

extern int func_00191000(char *a0, char *a1);
extern void func_001B0C60(int a0, int a1, int a2);
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_008106B8;

void func_00193EB0(char *arg0, char *arg1, int arg2) {
    int t;

    t = *(int *)(arg1 + 0x230);

    if (t == 8) goto blk1;
    if (t == 9) goto blk1;
    if (t == 7) goto blk1;
    if (t == 6) goto blk1;
    if (t == 0x2D) goto blk1;
    if (t == 0x2C) goto blk1;
    if (t == 1) goto blk2;
    if (t == 0x21) goto blk2;
    if (t == 0x12) goto blk3;
    if (t == 0x28) goto blk4;
    if (t == 0x29) goto blk5;
    if (t == 0xC) goto blk5;
    if (t == 0xD) goto blk7;
    if (t == 0x2A) goto blk7;
    if (t == 0x10) goto blk9;
    return;

blk9:
    *(char *)(arg0 + 6) = 9;
    *(char *)(arg0 + 1) = 0;
    return;

blk7:
    *(char *)(arg0 + 6) = 1;
    *(char *)(arg0 + 1) = 0;
    return;

blk5:
    *(char *)(arg0 + 6) = 2;
    *(char *)(arg0 + 1) = 0;
    return;

blk4:
    *(char *)(arg0 + 6) = 0xE;
    *(char *)(arg0 + 1) = 0;
    return;

blk3:
    *(char *)(arg0 + 6) = 0xB;
    *(char *)(arg0 + 1) = 0;
    return;

blk2:
    if (arg2 != 2 && *(unsigned char *)(arg0 + 5) == 0 && *(unsigned char *)(arg0 + 6) == 0) {
        func_00191000(arg0, arg1);
    }
    return;

blk1:
    if (D_00810700 == 0x16) {
        *(char *)(arg0 + 6) = 0xC;
        *(char *)(arg0 + 1) = 0;
        return;
    }
    if (D_00810700 == 0x13) {
        if (D_00810701 != 0) {
            *(char *)(arg0 + 6) = 0xD;
            *(char *)(arg0 + 1) = 0;
            return;
        }
        if (D_008106B8 != 0) {
            return;
        }
        if (*(unsigned char *)0x70003B8D != 0) {
            return;
        }
        if (*(float *)(arg1 + 0xA0) < 872.0f) {
            if (*(float *)(arg1 + 0xA4) <= 356.0f) {
                func_001B0C60(0x13, 0, 0xD);
                *(char *)(arg0 + 6) = 7;
            }
            return;
        }
        if (*(float *)(arg1 + 0xA8) <= 900.0f) {
            if (*(float *)(arg1 + 0xA4) <= 356.0f) {
                func_001B0C60(0x13, 0, 0xB);
                *(char *)(arg0 + 6) = 7;
            }
            return;
        }
        if (*(float *)(arg1 + 0xA4) <= 365.0f) {
            func_001B0C60(0x13, 0, 0xC);
            *(char *)(arg0 + 6) = 7;
        }
        return;
    }
    if (D_00810700 != 0xD) {
        return;
    }
    if (D_008106B8 != 0) {
        return;
    }
    if (*(unsigned char *)0x70003B8D != 0) {
        return;
    }
    {
        int m = D_00810702;
        if (m == 4 || m == 6) {
            if (*(float *)(arg1 + 0xA4) <= 159.0f) {
                func_001B0C60(0x13, 0, 9);
                *(char *)(arg0 + 6) = 7;
            }
            return;
        }
        if (m == 5 || m == 7) {
            if (*(float *)(arg1 + 0xA4) <= 159.0f) {
                func_001B0C60(0x13, 0, 0xA);
                *(char *)(arg0 + 6) = 7;
            }
        }
    }
}
