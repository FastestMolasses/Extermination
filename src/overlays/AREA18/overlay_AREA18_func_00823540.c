// CFLAGS: -O4,p -sdatathreshold 4
// Overlay AREA18 init function.  Registers this overlay with the boot ELF's
// area-manager subsystem by writing the area-type constant, a pointer to the
// overlay's data section, and two zeroes into gp-relative slots in boot BSS.
extern int D_00275C1C;
extern int D_00275C24;
extern int D_00275C28;
extern int D_00275C2C;
extern char D_overlay_AREA18_00823D80[];

void overlay_AREA18_func_00823540(void) {
    D_00275C28 = 0x20;
    D_00275C1C = (int)D_overlay_AREA18_00823D80;
    D_00275C2C = 0;
    D_00275C24 = 0;
}
