// CFLAGS: -O4,p -sdatathreshold 4
// if (a0[0x15] != 0) { G->x8 = 0; } else { G->x8 = 0xFFFF; }
extern int *D_00275CA8;

void func_overlay_AREA13_00827D90(short *a0) {
    if (a0[0x15] != 0) {
        D_00275CA8[2] = 0;
    } else {
        D_00275CA8[2] = 0xFFFF;
    }
}
