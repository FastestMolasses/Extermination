// CFLAGS: -O4,p -sdatathreshold 0
// Two-call wrapper: register two memory blocks then return 1.
extern int func_1EFD20(int code, char *ptr);
extern char D_overlay_AREA00_0082B780[];
extern char D_AC2D40[];

int func_overlay_AREA00_00826070(void) {
    func_1EFD20(0x8000005D, D_overlay_AREA00_0082B780);
    func_1EFD20(0x80000004, D_AC2D40);
    return 1;
}
