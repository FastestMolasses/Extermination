// COMPILER: eegcc
// CFLAGS: -O2
//
// MPEG picture-skip entry. Emits the "odd number of field pictures" diagnostic once when a
// frame picture (D_002414AC == 3) arrives with the pending-field flag D_0024141C set, picks
// the destination frame buffer for the current picture structure, skips to the next picture
// and, on success, marks that buffer's +0x28 field.
//
// ee-gcc note: gcc expands the switch BODIES in source order and then moves the dispatch
// tree to the front (reorder_insns), so the case order must be 3, 1, 2, default — that is
// what turns the case-3 test into the target's `bne v1,3 -> default` with case 3 falling
// through, rather than `beq v1,3 -> case3`.

extern int D_0024141C;
extern int D_002414AC;
extern unsigned char *D_00241544;
extern unsigned char *D_00241548;
extern unsigned char *D_0024154C;
extern char D_0026B508[];
extern char D_0026B528[];

extern void func_0010A3A8(char *);
extern int sub_Skip_to_the_next_picture(int);

int sub_odd_number_of_field_pictures(int arg)
{
    unsigned char *dst;
    int ret;

    if (D_002414AC == 3) {
        if (D_0024141C != 0) {
            func_0010A3A8(D_0026B508);
            D_0024141C = 0;
        }
    }

    switch (D_002414AC) {
    case 3:
        dst = D_00241544;
        break;
    case 1:
        dst = D_00241548;
        break;
    case 2:
        dst = D_0024154C;
        break;
    default:
        dst = D_00241544;
        func_0010A3A8(D_0026B528);
        break;
    }

    ret = sub_Skip_to_the_next_picture(arg);
    if (ret != 0)
        *(int *)(dst + 0x28) = 1;
    return ret;
}
