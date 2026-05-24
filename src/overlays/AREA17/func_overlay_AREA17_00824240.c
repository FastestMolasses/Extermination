// CFLAGS: -O4,p -sdatathreshold 0
// Init wrapper: 3-call setup, returns 1.
extern int func_1B6660(char *ptr);
extern int func_1EFD20(int code, char *ptr);
extern char D_overlay_AREA17_00827520[];
extern char D_overlay_AREA17_00969A90[];
extern char D_overlay_AREA17_00826500[];

int func_overlay_AREA17_00824240(void) {
    func_1EFD20(0x8000005C, D_overlay_AREA17_00827520);
    func_1EFD20(0x80000004, D_overlay_AREA17_00969A90);
    func_1B6660(D_overlay_AREA17_00826500);
    return 1;
}
