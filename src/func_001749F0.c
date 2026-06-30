// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Compares a 16-bit value against the field at p+0x20C; if different,
// updates the field and returns 1 (changed), else returns 0 (unchanged).
// First forwards (p, v) to func_001C67E0.
extern void func_001C67E0(void *p, int v);

int func_001749F0(short *p, int v) {
    func_001C67E0(p, v);
    if ((short) v != p[0x106]) {
        p[0x106] = (short) v;
        return 1;
    }
    return 0;
}
