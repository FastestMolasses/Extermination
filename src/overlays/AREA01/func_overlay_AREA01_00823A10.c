// CFLAGS: -O4,p -sdatathreshold 4
extern int D_00275C18;
extern int D_00275C1C;
extern int D_00275C20;
extern int D_00275C24;
extern int D_00275C28;
extern int D_00275C2C;
extern char D_overlay_AREA01_0082CD00[];
extern char D_overlay_AREA01_00836D80[];

void func_overlay_AREA01_00823A10(void) {
    D_00275C2C = 1;
    D_00275C28 = 0x20;
    D_00275C20 = (int)D_overlay_AREA01_0082CD00;
    D_00275C24 = 0;
    D_00275C18 = 0;
    D_00275C1C = (int)D_overlay_AREA01_00836D80;
}
