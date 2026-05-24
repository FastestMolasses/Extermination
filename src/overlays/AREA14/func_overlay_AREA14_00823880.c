// CFLAGS: -O4,p -sdatathreshold 4
// Thin wrapper: call func_1FAE70(0), return 1.
extern void func_1FAE70(int);

int func_overlay_AREA14_00823880(void) {
    func_1FAE70(0);
    return 1;
}
