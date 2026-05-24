// CFLAGS: -O4,p -sdatathreshold 4
// Save a2->8 to gp-rel D_00275CA4, return 1.
extern int D_00275CA4;

int func_overlay_AREA03_008262F0(int a0, int a1, int *a2) {
    D_00275CA4 = a2[2];
    return 1;
}
