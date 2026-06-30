// NEARMISS func_0012E0B0  (vram 0x0012E0B0, 0x1A4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.05% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// CW branch-target-alignment nop: original CodeWarrior pads a trailing `nop` (in the bnez delay slot) before the shared `return 0` merge after the second (slots 4..7) loop; mwcc (both 991202 and 2.3.3) omits this pad. Sole residual after body byte-identical. Known CW-vs-mwcc scheduling wall, NOT th...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Slot-allocation helper for object arg0 against the global record at D_008102B0.
// Bails (returns 0) when the global gate D_70003B8D is set, when func_0021BD60
// reports the record busy, when D_008106BC is set, or when arg0+0xB (a count/flag)
// is zero. Otherwise it measures func_0011DF78(abs-angle) of the heading delta
// (D_008102B0+0xC4 minus arg0+0xC4, normalized by func_001B1470): if that
// half-turn-bounded angle exceeds PI/2 (1.5707964f) it scans free-bit slots 0..3,
// else slots 4..7, in the bitfield byte D_0081083C. On the first free bit it
// claims it, stores (slot+0x80) as the short at arg1+0xF6, sets arg0[0]=2, ORs 2
// into D_008102B0[0], and returns 1. Returns 0 if no slot is free.
//
// NEARMISS: mwcc 2.3.3 reaches 99.048 (991202 84.905). Body byte-identical; the
// sole residual is a trailing CW branch-target-alignment `nop` the original
// CodeWarrior pads before the shared `return 0` merge after the second (4..7)
// loop. mwcc (both builds) omits this pad -- a known CW-vs-mwcc scheduling wall
// that 2.3.3 does not fix. Logic fully recovered.
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
        return 0;
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
        return 0;
    }
}
