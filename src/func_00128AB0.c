// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Entity init/refresh handler. Selects a capability check based on the global
// state byte D_00810788: if ==0xFF check slot 0x10, else slot 0xF (both vs 0x11)
// via func_001B10B0; on failure return 0. Sets the a1+0xE1 ready flag (1 in the
// 0xFF branch, 0 otherwise). Then if a0+0xD has bit7 set: a1+0xE0=1 and clear
// bit7 of a0+0xD, else a1+0xE0=0. Finally chains func_001289C0(a0,a1) and
// func_00102948(a1+0x50, a0+0xB0); returns 1.
extern int func_001B10B0(unsigned char *, int, int);
extern void func_001289C0(unsigned char *, unsigned char *);
extern void func_00102948(unsigned char *, unsigned char *);

extern unsigned char D_00810788;

int func_00128AB0(unsigned char *a0, unsigned char *a1) {
    if (D_00810788 == 0xFF) {
        if (func_001B10B0(a0, 0x10, 0x11) != 0) {
            return 0;
        }
        a1[0xE1] = 1;
    } else {
        if (func_001B10B0(a0, 0xF, 0x11) != 0) {
            return 0;
        }
        a1[0xE1] = 0;
    }
    if (a0[0xD] & 0x80) {
        a1[0xE0] = 1;
        a0[0xD] &= 0x7F;
    } else {
        a1[0xE0] = 0;
    }
    func_001289C0(a0, a1);
    func_00102948(a1 + 0x50, a0 + 0xB0);
    return 1;
}
