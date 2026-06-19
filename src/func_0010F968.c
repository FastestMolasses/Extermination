// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): set up a transfer descriptor and kick the DMA helper.
extern int D_00241C90;
extern int D_00279300;
extern int D_00279280;
extern char D_00279240[];
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int a4, void *a5,
                         int a6, int a7, int a8);

int func_0010F968(int a0) {
    if (D_00241C90 < 0) {
        return 0;
    }
    D_00279300 = a0;
    if (func_0010E8A8(D_00279240, 2, 0, &D_00279300, 4, &D_00279280, 4, 0, 0) < 0) {
        return -1;
    }
    return D_00279280;
}
