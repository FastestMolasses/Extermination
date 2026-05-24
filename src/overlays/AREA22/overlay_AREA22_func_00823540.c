// CFLAGS: -O4,p -sdatathreshold 4
extern int D_00275C1C;
extern int D_00275C24;
extern int D_00275C28;
extern int D_00275C2C;
extern char D_overlay_AREA22_00823E00[];

void overlay_AREA22_func_00823540(void) {
    D_00275C28 = 0x20;
    D_00275C1C = (int)D_overlay_AREA22_00823E00;
    D_00275C2C = 0;
    D_00275C24 = 0;
}
