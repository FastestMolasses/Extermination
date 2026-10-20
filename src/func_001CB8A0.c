// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Starts an empty DMA chain at D_0028F700 + D_00810E80 * 0x70000 + 0x1F3EC0 +
// a1 * 64 (tag 0x20000000) and returns the 28-bit addresses of the chain and
// of its next entry through a2 / a3 (the loop-free sibling of func_001CB800).
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 3, 2026-10-20).

extern char D_0028F700;
extern short D_00810E80;

void func_001CB8A0(int *arg0, int arg1, int *arg2, int *arg3) {
    char *base;

    base = &D_0028F700 + (int)D_00810E80 * 0x70000 + 0x1F3EC0;
    base = base + (arg1 << 6);
    *(int *)base = 0x20000000;
    ((int *)base)[1] = (long long)(base + 0x20) & 0xFFFFFFF;
    *arg2 = (long long)base & 0xFFFFFFF;
    *arg3 = (long long)(base + 0x20) & 0xFFFFFFF;
}
