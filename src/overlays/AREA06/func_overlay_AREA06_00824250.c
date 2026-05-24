// CFLAGS: -O4,p -sdatathreshold 4
extern int D_00275C1C;
extern int D_00275C20;
extern int D_00275C24;
extern int D_00275C28;
extern int D_00275C2C;
extern char D_overlay_AREA06_00828F00[];
extern char D_overlay_AREA06_00832F80[];

void func_overlay_AREA06_00824250(void) {
    D_00275C2C = 1;
    D_00275C28 = 0x20;
    D_00275C20 = (int)D_overlay_AREA06_00828F00;
    D_00275C24 = 0;
    D_00275C1C = (int)D_overlay_AREA06_00832F80;
}
