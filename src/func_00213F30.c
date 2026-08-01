// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Rebuilds the selectable-id list for a menu/roster widget.
//
// Clears the count byte at arg0+0x18, then maps the state byte at arg0+0x12 to
// an id range [lo,hi): {0:[0,0x20), 1:[0x20,0x32), 2:[0x32,0x48),
// 3:[0x48,0x5D), 4:[0x5D,0x6D)}; any other state keeps the caller-supplied
// lo/hi. For each id i in the range it appends one byte to the list at
// arg0+0x50, indexed by the count at arg0+0x18 (post-incremented in place):
// the id itself when the unlock/gate table entry D_00810700[i + 0x5C3] is set,
// otherwise the sentinel 0x6D ("locked"/blank slot). Finally zeroes the
// selection cursor at arg0+0x19.
extern unsigned char D_00810700[];

void func_00213F30(char *arg0, int a1u, int a2u, int lo, int hi) {
    int i;

    *(unsigned char *)(arg0 + 0x18) = 0;
    switch (*(unsigned char *)(arg0 + 0x12)) {
    case 0: lo = 0;    hi = 0x20; break;
    case 1: lo = 0x20; hi = 0x32; break;
    case 2: lo = 0x32; hi = 0x48; break;
    case 3: lo = 0x48; hi = 0x5D; break;
    case 4: lo = 0x5D; hi = 0x6D; break;
    }
    for (i = lo; i < hi; i++) {
        if (D_00810700[i + 0x5C3] != 0) {
            *(char *)(arg0 + (*(unsigned char *)(arg0 + 0x18))++ + 0x50) = i;
        } else {
            *(char *)(arg0 + (*(unsigned char *)(arg0 + 0x18))++ + 0x50) = 0x6D;
        }
    }
    *(char *)(arg0 + 0x19) = 0;
}
