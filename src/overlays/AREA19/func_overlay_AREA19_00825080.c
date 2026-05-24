// CFLAGS: -O4,p -sdatathreshold 4
extern int D_00275C1C;
extern int D_00275C20;
extern int D_00275C24;
extern int D_00275C28;
extern int D_00275C2C;
extern char D_overlay_AREA19_0082F880[];
extern char D_overlay_AREA19_0084D9C0[];

void func_overlay_AREA19_00825080(void) {
    D_00275C2C = 3;
    D_00275C28 = 0x20;
    D_00275C20 = (int)D_overlay_AREA19_0082F880;
    D_00275C24 = 0;
    D_00275C1C = (int)D_overlay_AREA19_0084D9C0;
}
