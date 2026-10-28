// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 93.41% NEARMISS).
// Fixes read from the original instructions:
//  - the table row index is a char: its two conditional moves then keep the
//    "1" value as the base and clear it on the zero test (an int index flipped
//    both to the opposite polarity);
//  - the "D_0024147C == 2" test in the table branch compares against a local
//    holding 2, and the four table words are stored in natural order;
//  - the constant-1 flag of blocks 1 and 3 (re-set to 1 in block 3) and the
//    table branch's D_002414AC == 2 flag are separate variables; one shared
//    variable swapped the sel pointer and that flag between two saved registers.
//
// Issues func_00103DC8 calls for the current motion
// type mt (1, 2 or 3; anything else reports through func_0010A378), first for the
// flags & 8 pass (or when D_0024147C == 2), then for the flags & 4 pass, after
// clearing the active slot's word at +0x12C.

extern unsigned char D_002418F8[];
extern int D_00241300;
extern int D_00241304;
extern int D_0024130C;
extern int D_00241310;
extern int D_00241318;
extern int D_0024131C;
extern int D_0024141C;
extern int D_0024147C;
extern int D_002414AC;
extern char D_0026B308[];
extern char D_0026B328[];
extern char D_0026B348[];

extern void func_00103DC8(int, int, int, int, int, int, int, int, int, int, int);
extern void func_00105390(int *, int, int, int);
extern void func_0010A378(char *, int);

void sub_c_invalid_motion_type_d_2(int ctx, int arg, int flags, int mt, int *src, int *sel, int extra)
{
    unsigned char *base;
    int out[4];
    int tbl[2][2];
    int done;
    char idx;
    int on;      /* constant 1 held in a variable (block 1 and block 3) */
    int fld;     /* D_002414AC == 2 */
    int one;
    int two;

    on = 1;
    base = D_002418F8;
    done = 0;
    *(int *)(base + *(int *)(base + 0x280) * 0x140 + 0x12C) = 0;

    if ((flags & 8) || D_0024147C == 2) {
        if (D_002414AC == 3) {
            if (mt == 2 || !(flags & 8)) {
                func_00103DC8(D_00241300, 0, 0, 0, 0x10, ctx, arg, src[0], src[1], 0, 0);
            } else if (mt == on) {
                func_00103DC8(D_00241300, sel[0], 0, 0, 8, ctx, arg, src[0], src[1] >> 1, mt, 0);
                func_00103DC8(D_00241300, sel[2], 1, 0, 8, ctx, arg, src[4], src[5] >> 1, mt, 0);
            } else if (mt == 3) {
                func_00105390(out, extra, src[0], src[1] >> 1);
                func_00103DC8(D_00241300, 0, 0, 0, 8, ctx, arg, src[0], src[1] >> 1, on, 0);
                func_00103DC8(D_00241300, 1, 0, 0, 8, ctx, arg, out[0], out[1], on, on);
                func_00103DC8(D_00241300, 1, 1, 0, 8, ctx, arg, src[0], src[1] >> 1, on, 0);
                func_00103DC8(D_00241300, 0, 1, 0, 8, ctx, arg, out[2], out[3], on, on);
            } else {
                func_0010A378(D_0026B308, mt);
            }
        } else {
            tbl[0][0] = D_0024130C;
            tbl[0][1] = D_00241318;
            tbl[1][0] = D_00241310;
            tbl[1][1] = D_0024131C;
            two = 2;
            fld = (D_002414AC == 2);
            idx = 0;
            if (D_0024147C == two && D_0024141C != 0) {
                idx = (fld != sel[0]);
            }
            one = 1;
            if (mt == one || !(flags & 8)) {
                func_00103DC8(tbl[idx][sel[0]], 0, 0, 0, 0x10, ctx, arg, src[0], src[1], 0, 0);
            } else if (mt == 2) {
                func_00103DC8(tbl[idx][sel[0]], 0, 0, 0, 8, ctx, arg, src[0], src[1], 0, 0);
                idx = 0;
                if (D_0024147C == mt && D_0024141C != 0) {
                    idx = (fld == sel[2]) ? 0 : one;
                }
                func_00103DC8(tbl[idx][sel[2]], 0, 0, 8, 8, ctx, arg, src[4], src[5], 0, 0);
            } else if (mt == 3) {
                idx = (D_0024141C == 0) ? 0 : one;
                func_00105390(out, extra, src[0], src[1]);
                func_00103DC8(tbl[0][fld], 0, 0, 0, 0x10, ctx, arg, src[0], src[1], 0, 0);
                func_00103DC8(tbl[idx][fld ? 0 : 1], 0, 0, 0, 0x10, ctx, arg, out[0], out[1], 0, one);
            } else {
                func_0010A378(D_0026B328, mt);
            }
        }
        done = 1;
    }

    if (flags & 4) {
        if (D_002414AC == 3) {
            on = 1;
            if (mt == 2) {
                func_00103DC8(D_00241304, 0, 0, 0, 0x10, ctx, arg, src[2], src[3], 0, done);
            } else {
                func_00103DC8(D_00241304, sel[1], 0, 0, 8, ctx, arg, src[2], src[3] >> 1, on, done);
                func_00103DC8(D_00241304, sel[3], 1, 0, 8, ctx, arg, src[6], src[7] >> 1, on, done);
            }
        } else if (mt == 1) {
            func_00103DC8(sel[1] ? D_0024131C : D_00241310, 0, 0, 0, 0x10, ctx, arg, src[2], src[3], 0, done);
        } else if (mt == 2) {
            func_00103DC8(sel[1] ? D_0024131C : D_00241310, 0, 0, 0, 8, ctx, arg, src[2], src[3], 0, done);
            func_00103DC8(sel[3] ? D_0024131C : D_00241310, 0, 0, 8, 8, ctx, arg, src[6], src[7], 0, done);
        } else {
            func_0010A378(D_0026B348, mt);
        }
    }
}
