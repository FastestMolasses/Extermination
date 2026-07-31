// NEARMISS sub_c_invalid_motion_type_d_2  (vram 0x_invalid_motion_type_d_2, 0x6B0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.41% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc callee-saved coloring tie-break: body is instruction-for-instruction identical (438/438) but s6<->s7 are swapped (target s6=sel param / s7=fld+eye; ours s6=fld+eye / s7=sel), plus ~6 delay-slot/scheduling slots that follow from that swap and 2 movz-vs-movn conditional-move direction picks. ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

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
    int idx;
    int fld;
    int one;

    fld = 1;
    base = D_002418F8;
    done = 0;
    *(int *)(base + *(int *)(base + 0x280) * 0x140 + 0x12C) = 0;

    if ((flags & 8) || D_0024147C == 2) {
        if (D_002414AC == 3) {
            if (mt == 2 || !(flags & 8)) {
                func_00103DC8(D_00241300, 0, 0, 0, 0x10, ctx, arg, src[0], src[1], 0, 0);
            } else if (mt == fld) {
                func_00103DC8(D_00241300, sel[0], 0, 0, 8, ctx, arg, src[0], src[1] >> 1, mt, 0);
                func_00103DC8(D_00241300, sel[2], 1, 0, 8, ctx, arg, src[4], src[5] >> 1, mt, 0);
            } else if (mt == 3) {
                func_00105390(out, extra, src[0], src[1] >> 1);
                func_00103DC8(D_00241300, 0, 0, 0, 8, ctx, arg, src[0], src[1] >> 1, fld, 0);
                func_00103DC8(D_00241300, 1, 0, 0, 8, ctx, arg, out[0], out[1], fld, fld);
                func_00103DC8(D_00241300, 1, 1, 0, 8, ctx, arg, src[0], src[1] >> 1, fld, 0);
                func_00103DC8(D_00241300, 0, 1, 0, 8, ctx, arg, out[2], out[3], fld, fld);
            } else {
                func_0010A378(D_0026B308, mt);
            }
        } else {
            tbl[0][0] = D_0024130C;
            tbl[0][1] = D_00241318;
            tbl[1][1] = D_0024131C;
            tbl[1][0] = D_00241310;
            fld = (D_002414AC == 2);
            idx = 0;
            if (D_0024147C == 2 && D_0024141C != 0) {
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
            fld = 1;
            if (mt == 2) {
                func_00103DC8(D_00241304, 0, 0, 0, 0x10, ctx, arg, src[2], src[3], 0, done);
            } else {
                func_00103DC8(D_00241304, sel[1], 0, 0, 8, ctx, arg, src[2], src[3] >> 1, fld, done);
                func_00103DC8(D_00241304, sel[3], 1, 0, 8, ctx, arg, src[6], src[7] >> 1, fld, done);
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
