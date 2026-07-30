// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// SEMANTICS:
//   Nested pairwise-interaction sweep over two global object lists (an O(n*m)
//   "for each active A, for each active B" pass). Both lists are arrays of object
//   pointers with a live count held in a gp-relative short:
//     D_00275BB0 / D_00275BB8 -> outer list base / count
//     D_00275B90 / D_00275B98 -> inner list base / count
//   Bit 0 of each object's byte +0 is the "active" flag; byte +3 is its kind/type
//   tag. Early-out: if the inner count is zero nothing can pair, so the whole
//   sweep is skipped up front. The inner count and base are RE-READ from the
//   globals on every outer iteration (the callee may resize the list).
//   For each active outer object, every active inner object whose kind tag is
//   0,1,4,5,6 or 7 is handed to func_001A9C40(outer, inner); kinds 2 and 3, and
//   every tag >= 8, are skipped.
//   The loop counters are plain `int` in the original, not `short` -- decrementing
//   a short would force a dsll32/dsra32 sign-extend pair that the target lacks.
//   Local declaration order below is load-bearing: mwcc assigns saved registers
//   in declaration order, and this order reproduces the target's
//   s0=outer_obj / s1=inner_pp / s2=outer_pp / s3=outer_left / s4=inner_left.

extern void func_001A9C40(unsigned char *a, unsigned char *b);

extern unsigned char **D_00275B90;
extern short D_00275B98;
extern unsigned char **D_00275BB0;
extern short D_00275BB8;

void func_001A9D20(void)
{
    int outer_left;
    int inner_left;
    unsigned char *outer_obj;
    unsigned char **inner_pp;
    unsigned char **outer_pp;
    unsigned char *inner_obj;

    if (D_00275B98 != 0) {
        outer_left = D_00275BB8;
        outer_pp = D_00275BB0;
        while (outer_left != 0) {
            outer_obj = *outer_pp;
            outer_left--;
            outer_pp++;
            if ((outer_obj[0] & 1) != 0) {
                inner_left = D_00275B98;
                inner_pp = D_00275B90;
                while (inner_left != 0) {
                    inner_obj = *inner_pp;
                    inner_left--;
                    inner_pp++;
                    if ((inner_obj[0] & 1) != 0) {
                        switch (inner_obj[3]) {
                        case 0:
                        case 1:
                        case 4:
                        case 5:
                        case 6:
                        case 7:
                            func_001A9C40(outer_obj, inner_obj);
                            break;
                        }
                    }
                }
            }
        }
    }
}
