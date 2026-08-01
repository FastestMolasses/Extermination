// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Slot-allocation helper for object arg0 against the global record at D_008102B0.
// Bails (returns 0) when the scratchpad gate at 0x70003B8D is set, when
// func_0021BD60 reports the record busy, when D_008106BC is set, or when the
// count/flag byte at arg0+0xB is zero. Otherwise it measures
// func_0011DF78(func_001B1470(heading delta)) -- the half-turn-normalized
// absolute angle between D_008102B0+0xC4 and arg0+0xC4. If that angle exceeds
// PI/2 (1.5707964f) it scans free bits 0..3 of the bitfield byte D_0081083C,
// otherwise bits 4..7. On the first free bit it claims the bit, stores
// (slot + 0x80) as the short at arg1+0xF6, sets arg0[0] = 2, ORs 2 into
// D_008102B0[0], and returns 1. Returns 0 when no slot is free.
//
// Match note: the two scan loops must SHARE a single trailing `return 0;`
// (both arms of the if/else fall out of their loop). Giving each arm its own
// `return 0;` loses the trailing branch-target `nop` before the merge and caps
// at 99.048 -- that residual had been recorded as a CW-vs-mwcc
// branch-target-alignment wall; it is a source shape, not a wall.
extern int func_0021BD60(void *p);
extern float func_001B1470(float a);
extern float func_0011DF78(float a);
extern char D_008102B0;
extern unsigned char D_008106BC;
extern unsigned char D_0081083C;

int func_0012E0B0(char *arg0, char *arg1) {
    char *g = &D_008102B0;
    int i;
    int bit;

    if (*(unsigned char *)0x70003B8D != 0) {
        return 0;
    }
    if (func_0021BD60(g) != 0) {
        return 0;
    }
    if (D_008106BC != 0) {
        return 0;
    }
    if (*(unsigned char *)(arg0 + 0xB) == 0) {
        return 0;
    }
    if (!(func_0011DF78(func_001B1470(*(float *)(g + 0xC4) - *(float *)(arg0 + 0xC4))) <= 1.5707964f)) {
        for (i = 0; i < 4; i++) {
            bit = 1 << i;
            if (!(D_0081083C & bit)) {
                D_0081083C |= bit;
                *(short *)(arg1 + 0xF6) = i + 0x80;
                *(char *)(arg0 + 0) = 2;
                *(unsigned char *)(g + 0) = *(unsigned char *)(g + 0) | 2;
                return 1;
            }
        }
    } else {
        for (i = 4; i < 8; i++) {
            bit = 1 << i;
            if (!(D_0081083C & bit)) {
                D_0081083C |= bit;
                *(short *)(arg1 + 0xF6) = i + 0x80;
                *(char *)(arg0 + 0) = 2;
                *(unsigned char *)(g + 0) = *(unsigned char *)(g + 0) | 2;
                return 1;
            }
        }
    }
    return 0;
}
