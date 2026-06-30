// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Build a transform scratch (func_001028D0 into a 16-byte stack temp), then
// apply it (func_001028B8) to the entity's three 0xA0/0xB0/0xC0 vector slots
// and to the shared workspace D_70003B40, finishing with func_00102948 into
// D_70003B50. 2nd arg of func_001028D0 is the caller's leftover 2nd param.
extern void func_001028D0(void *a, void *b, void *c);
extern void func_001028B8(void *a, void *b, void *c);
extern void func_00102948(void *a, void *b);
extern char D_70003B40[];
extern char D_70003B50[];

void func_00182F90(char *p, void *a1) {
    char tmp[16];
    func_001028D0(tmp, a1, p + 0xA0);
    func_001028B8(p + 0xA0, p + 0xA0, tmp);
    func_001028B8(p + 0xB0, p + 0xB0, tmp);
    func_001028B8(D_70003B40, D_70003B40, tmp);
    func_00102948(D_70003B50, p + 0xC0);
}
