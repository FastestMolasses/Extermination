// CFLAGS: -O4,p -sdatathreshold 4
extern int D_00275C18;
extern int D_00275C1C;
extern int D_00275C20;
extern int D_00275C24;
extern int D_00275C28;
extern int D_00275C2C;
extern char D_overlay_AREA00_0082D480[];
extern char D_overlay_AREA00_00841540[];
extern char D_overlay_AREA00_00982140[];

void func_overlay_AREA00_00824DC0(void) {
    D_00275C2C = 2;
    D_00275C28 = 0x20;
    D_00275C24 = 0x20;
    D_00275C20 = (int)D_overlay_AREA00_0082D480;
    D_00275C1C = (int)D_overlay_AREA00_00841540;
    D_00275C18 = (int)D_overlay_AREA00_00982140;
}
