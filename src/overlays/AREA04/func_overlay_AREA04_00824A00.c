// CFLAGS: -O4,p -sdatathreshold 0
// Init wrapper: register a block, install two memory regions, return 1.
extern int func_1B6660(char *ptr);
extern int func_1EFD20(int code, char *ptr);
extern char D_overlay_AREA04_00827230[];
extern char D_overlay_AREA04_00828B90[];

int func_overlay_AREA04_00824A00(void) {
    int buf[4];
    func_1B6660(D_overlay_AREA04_00827230);
    func_1EFD20(0x8000005B, D_overlay_AREA04_00828B90);
    func_1EFD20(0x80000004, (char *)buf);
    return 1;
}
