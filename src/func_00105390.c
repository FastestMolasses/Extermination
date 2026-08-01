// COMPILER: eegcc
// CFLAGS: -O2

// Fills a coordinate list from the base point in[0..1] and the cell size
// (a2, a3), using the halving idiom (n > 0 ? n + 1 : n) >> 1 and its
// three-halves counterpart ((n * 3) + (n > 0)) >> 1.
// D_002414AC selects the layout: 3 = four entries out[0..3], where
// D_002414B0 swaps which axis pair gets the half and which the
// three-halves offset; any other value = two entries out[0..1], with
// out[1] biased -1 when D_002414AC == 1 and +1 otherwise.

extern int D_002414AC;
extern int D_002414B0;

void func_00105390(int *out, int *in, int a2, int a3)
{
    if (D_002414AC == 3) {
        if (D_002414B0 != 0) {
            out[0] = ((a2 > 0 ? a2 + 1 : a2) >> 1) + in[0];
            out[1] = ((a3 > 0 ? a3 + 1 : a3) >> 1) + in[1] - 1;
            out[2] = (((a2 * 3) + (a2 > 0 ? 1 : 0)) >> 1) + in[0];
            out[3] = (((a3 * 3) + (a3 > 0 ? 1 : 0)) >> 1) + in[1] + 1;
        } else {
            out[0] = (((a2 * 3) + (a2 > 0 ? 1 : 0)) >> 1) + in[0];
            out[1] = (((a3 * 3) + (a3 > 0 ? 1 : 0)) >> 1) + in[1] - 1;
            out[2] = ((a2 > 0 ? a2 + 1 : a2) >> 1) + in[0];
            out[3] = ((a3 > 0 ? a3 + 1 : a3) >> 1) + in[1] + 1;
        }
    } else {
        int v;
        out[0] = ((a2 > 0 ? a2 + 1 : a2) >> 1) + in[0];
        v = ((a3 > 0 ? a3 + 1 : a3) >> 1) + in[1];
        out[1] = v;
        if (D_002414AC == 1) {
            out[1] = v - 1;
        } else {
            out[1] = v + 1;
        }
    }
}
