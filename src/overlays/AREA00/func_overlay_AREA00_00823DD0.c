// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00823E10 (splat/link name 00823DD0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: in state 1 spawns effect 0x80000014 through func_001F00A0 at
//  +0x100 / +0xC0 and, when one is returned, resets its matrix and copies
//  +0x100 into it.
extern char *func_001F00A0(int id, void *pos, void *rot, int a);
extern void func_001029C0(void *m);
extern void func_00102948(void *dst, void *src);

void func_overlay_AREA00_00823DD0(unsigned char *self) {
    char *e;
    switch (self[4]) {
    case 0:
        break;
    case 1:
        e = func_001F00A0(0x80000014, self + 0x100, self + 0xC0, 0);
        if (e != 0) {
            func_001029C0(e + 0xD0);
            func_00102948(e + 0x100, self + 0x100);
        }
        break;
    case 2:
    case 3:
        break;
    }
}
