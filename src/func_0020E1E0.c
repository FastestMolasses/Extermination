// CFLAGS: -O4,p -sdatathreshold 0
// Allocates an object (func_001AFF10), and if non-null wires up its update
// callback (func_0020E460), registers it (func_001C6120/func_001CA6E0) and
// stores a derived id and the owner index into byte fields 0xC/0xD.
extern char *func_001AFF10(void);
extern int func_001C6120(int, int);
extern void func_001CA6E0(char *, int);
extern int func_001C6150(int);
extern void func_0020E460(void);
extern int D_0028A56C;

void func_0020E1E0(int a0) {
    char *s0 = func_001AFF10();
    if (s0 != 0) {
        *(void (**)(void))(s0 + 0x10) = func_0020E460;
        func_001CA6E0(s0, func_001C6120(D_0028A56C, a0));
        s0[0xC] = (char)func_001C6150(*(int *)(s0 + 0x44));
        s0[0xD] = (char)a0;
    }
}
