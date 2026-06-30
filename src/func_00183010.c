// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Sibling of func_00182F90: applies the externally-supplied transform q
// (func_001028B8) to the entity's 0xA0/0xB0 vector slots and the shared
// workspace D_70003B40, then func_00102948 into D_70003B50 from slot 0xC0.
extern void func_001028B8(void *a, void *b, void *c);
extern void func_00102948(void *a, void *b);
extern char D_70003B40[];
extern char D_70003B50[];

void func_00183010(char *p, void *q) {
    func_001028B8(p + 0xA0, p + 0xA0, q);
    func_001028B8(p + 0xB0, p + 0xB0, q);
    func_001028B8(D_70003B40, D_70003B40, q);
    func_00102948(D_70003B50, p + 0xC0);
}
