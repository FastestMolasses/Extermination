// CFLAGS: -O4,p -sdatathreshold 0
extern int func_1B6660(char *ptr);
extern void func_1F6B90(void);
extern void func_19C6F0(int, int);
extern char D_overlay_AREA19_0082A9C0[];

int func_overlay_AREA19_00827AE0(void) {
    func_1B6660(D_overlay_AREA19_0082A9C0);
    func_1F6B90();
    func_19C6F0(8, 1);
    return 1;
}
