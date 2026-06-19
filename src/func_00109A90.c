// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): for each live context, clear its field 0x28; return 1.
extern int *D_00241300;
extern int *D_0024130C;
extern int *D_00241318;
extern int *D_00241304;
extern int *D_00241310;
extern int *D_0024131C;

int func_00109A90(void) {
    if (D_00241300) D_00241300[0x28 / 4] = 0;
    if (D_0024130C) D_0024130C[0x28 / 4] = 0;
    if (D_00241318) D_00241318[0x28 / 4] = 0;
    if (D_00241304) D_00241304[0x28 / 4] = 0;
    if (D_00241310) D_00241310[0x28 / 4] = 0;
    if (D_0024131C) D_0024131C[0x28 / 4] = 0;
    return 1;
}
