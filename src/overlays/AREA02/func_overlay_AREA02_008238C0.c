// CFLAGS: -O4,p -sdatathreshold 4
// AREA02 extended init: stores 0x20 to both D_00275C28 and D_00275C24, plus extra D_00275C18 pointer.
extern int D_00275C18;
extern int D_00275C1C;
extern int D_00275C24;
extern int D_00275C28;
extern int D_00275C2C;
extern char D_overlay_AREA02_00829280[];
extern char D_overlay_AREA02_00969E80[];

void func_overlay_AREA02_008238C0(void) {
    D_00275C28 = 0x20;
    D_00275C24 = 0x20;
    D_00275C1C = (int)D_overlay_AREA02_00829280;
    D_00275C2C = 0;
    D_00275C18 = (int)D_overlay_AREA02_00969E80;
}
