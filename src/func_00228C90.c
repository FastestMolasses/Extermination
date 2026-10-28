// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 86.40% NEARMISS).
// Fixes read from the original instructions:
//  - The record array is indexed as a struct array (D_00821500[i].name) in plain
//    for-loops, not a hand-hoisted pointer in while-loops. mwcc then strength-
//    reduces the index into the pointer after each loop guard, which gives the
//    original's saved-register colouring (arg0 in s0, &found in s2).
//  - The original reaches D_00275890 gp-relative: it is a small (<= 8 byte)
//    object, so it is declared with a size and the file uses
//    -sdatathreshold 8 (the other name strings are unsized externs).
//
// Scans arg0 records of D_00821500 (stride 0x40, name field at +0x20) and sets
// 8 "found" flags by matching each record name against expected names via
// func_00123020 (== 0 means match): flag0=D_00266F00, flag1=D_00275890,
// flag2=D_00273970, then one combined pass sets flag3..7 from D_00266F20/F60/
// FA0/FE0 and D_00267020. Returns 1 only if all 8 flags were found, else 0.
typedef struct { char pad[0x20]; char name[0x20]; } Rec;
extern int func_00123020(char *a, char *s);
extern Rec D_00821500[];
extern char D_00266F00[];
extern char D_00275890[8];
extern char D_00273970[];
extern char D_00266F20[];
extern char D_00266F60[];
extern char D_00266FA0[];
extern char D_00266FE0[];
extern char D_00267020[];
int func_00228C90(int arg0) {
    int found[8];
    int i;
    for (i = 0; i < 8; i++) {
        found[i] = 0;
    }
    for (i = 0; i < arg0; i++) {
        if (func_00123020(D_00821500[i].name, D_00266F00) == 0) {
            found[0] = 1;
        }
    }
    for (i = 0; i < arg0; i++) {
        if (func_00123020(D_00821500[i].name, D_00275890) == 0) {
            found[1] = 1;
        }
    }
    for (i = 0; i < arg0; i++) {
        if (func_00123020(D_00821500[i].name, D_00273970) == 0) {
            found[2] = 1;
        }
    }
    for (i = 0; i < arg0; i++) {
        if (func_00123020(D_00821500[i].name, D_00266F20) == 0) {
            found[3] = 1;
        }
        if (func_00123020(D_00821500[i].name, D_00266F60) == 0) {
            found[4] = 1;
        }
        if (func_00123020(D_00821500[i].name, D_00266FA0) == 0) {
            found[5] = 1;
        }
        if (func_00123020(D_00821500[i].name, D_00266FE0) == 0) {
            found[6] = 1;
        }
        if (func_00123020(D_00821500[i].name, D_00267020) == 0) {
            found[7] = 1;
        }
    }
    for (i = 0; i < 8; i++) {
        if (found[i] == 0) {
            return 0;
        }
    }
    return 1;
}
