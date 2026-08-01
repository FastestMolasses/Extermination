// COMPILER: eegcc
// CFLAGS: -O2

// SEMANTICS: Controller/pad-LED command dispatcher. Reads the pending command
// word at e[0x5E] and dispatches through jtbl_0026C0D0 (20 entries; 1,2,3,13,14
// and out-of-range fall into the default rescan).
//   0        - clear the command (e[0x5E]=0) and latch the port id e[3] into
//              e[0x5A]; skips the rescan.
//   4..11    - poke the shared device block *D_00281AC4: a read-modify-write of
//              the 16-bit register at +6 (cases 4..7) or +8 (cases 8..11), first
//              storing the masked value and then OR-ing in the new field:
//                4  -> byte at +6 zero-extended, then (0x7F - port) << 8
//                5  -> same, plus bit 0x8000
//                6  -> keep 0xFF0F, then ((0x7F - port) >> 3) << 4
//                7  -> keep 0xFFF0, then port >> 3 (unsigned)
//                8  -> keep 0x3F,   then (0x7F - port) << 6, then | (0x4000 - e[0x62])
//                9  -> as 8 plus bit 0x8000
//                10 -> keep 0xFFC0, then (0x7F - port) >> 2
//                11 -> as 10 plus bit 0x20
//              then falls into the rescan.
//   12       - e[0x62] = (port < 0x41) ? 0 : 0x4000, then rescan.
//   15/16/17/18,19 - call func_00119810 / func_00119828 / func_00119858 /
//              func_00119840 twice (slot 0 then slot 1) with the port id; these
//              skip the rescan.
//   default  - rescan the 0x30-entry table at D_0027CCC0 (stride 0x6A): for each
//              slot whose +2 field is 1, +4 field matches (e[0] & 0xF), +0x1A
//              field is 1, +6 field matches e[0x18], and (e[0x60]==0xFF or +0xE
//              matches e[0x60]), fire func_001157F0(3, slot, q[6], q[8]) where
//              q = *(void **)(D_00281AC0 + 4).
// Every path finishes with *(int *)(e+8) += 3.
// D_00281AC4 IS *(void **)(D_00281AC0 + 4) -- the case bodies address it as its
// own global while the rescan reaches it through D_00281AC0; both spellings are
// reproduced because that is how the target encodes them.
//
// Match notes (this had been recorded as an "ee-gcc loop-preheader / giv-init
// ORDERING" wall at 99.05 -- it is NOT a wall):
//  1. The scan preheader order is decided by ee-gcc's pre-reload haifa
//     scheduler: the insn carrying the base register's REG_DEAD note goes
//     first, the rest follow in RTL order. Loop-invariant constants hoisted
//     out of the loop are always appended at the END of the preheader, so
//     `r1 = base + 0xE` can never land between them -- unless the constant 1
//     is materialized by SOURCE in the preheader. `one = 1;` written between
//     `i = 0;` and `r1 = ...` puts `addiu $s5,$zero,1` in exactly the target's
//     slot. Permuting the initializer statements alone caps at 99.855; no
//     -O/-fno-* flag combination helps (all measured).
//  2. `t` caches the first loaded field so the third comparison reuses it
//     (`bnel $v0,$a0`), which is what CSE does in the target; comparing that
//     field against `one` a second time emits `bnel $v0,$s5` and also swaps
//     the $s4/$s5 colouring.
//  3. Measure against an expected object built through build.normalize_asm
//     (it appends build/jtblrodata/func_00118828.s). Assembling the raw splat
//     .s leaves jtbl_0026C0D0 undefined-external and caps the score at 99.955.
extern int func_001157F0(int a, int b, int c, int d);
extern int func_00119810(int a, int b);
extern int func_00119828(int a, int b, int c);
extern int func_00119840(int a, int b);
extern int func_00119858(int a, int b);
extern char D_0027CCC0[];
extern char D_00281AC0[];
extern char *D_00281AC4;

void func_00118828(char *e) {
    char *q;
    char *g;
    char *base;
    char *r1;
    char *r3;
    int i;
    int one;
    int t;

    switch (*(unsigned short *)(e + 0x5E)) {
    case 0:
        *(short *)(e + 0x5E) = 0;
        *(short *)(e + 0x5A) = *(unsigned char *)(e + 3);
        break;
    case 4:
        {
            char *p = D_00281AC4;
            int t = *(unsigned char *)(p + 6);
            *(short *)(p + 6) = t;
            t |= (0x7F - *(unsigned char *)(e + 3)) << 8;
            *(short *)(p + 6) = t;
        }
        goto scan;
    case 5:
        {
            char *p = D_00281AC4;
            int t = *(unsigned char *)(p + 6);
            *(short *)(p + 6) = t;
            t |= (0x7F - *(unsigned char *)(e + 3)) << 8;
            t |= 0x8000;
            *(short *)(p + 6) = t;
        }
        goto scan;
    case 6:
        {
            char *p = D_00281AC4;
            int t = *(unsigned short *)(p + 6) & 0xFF0F;
            *(short *)(p + 6) = t;
            t |= ((0x7F - *(unsigned char *)(e + 3)) >> 3) << 4;
            *(short *)(p + 6) = t;
        }
        goto scan;
    case 7:
        {
            char *p = D_00281AC4;
            int t = *(unsigned short *)(p + 6) & 0xFFF0;
            *(short *)(p + 6) = t;
            t |= *(unsigned char *)(e + 3) >> 3;
            *(short *)(p + 6) = t;
        }
        goto scan;
    case 8:
        {
            char *p = D_00281AC4;
            int t = *(unsigned short *)(p + 8) & 0x3F;
            *(short *)(p + 8) = t;
            t |= (0x7F - *(unsigned char *)(e + 3)) << 6;
            *(short *)(p + 8) = t;
            t |= 0x4000 - *(unsigned short *)(e + 0x62);
            *(short *)(p + 8) = t;
        }
        goto scan;
    case 9:
        {
            char *p = D_00281AC4;
            int t = *(unsigned short *)(p + 8) & 0x3F;
            *(short *)(p + 8) = t;
            t |= (0x7F - *(unsigned char *)(e + 3)) << 6;
            t |= 0x8000;
            *(short *)(p + 8) = t;
            t |= 0x4000 - *(unsigned short *)(e + 0x62);
            *(short *)(p + 8) = t;
        }
        goto scan;
    case 10:
        {
            char *p = D_00281AC4;
            int t = *(unsigned short *)(p + 8) & 0xFFC0;
            *(short *)(p + 8) = t;
            t |= (0x7F - *(unsigned char *)(e + 3)) >> 2;
            *(short *)(p + 8) = t;
        }
        goto scan;
    case 11:
        {
            char *p = D_00281AC4;
            int t = *(unsigned short *)(p + 8) & 0xFFC0;
            *(short *)(p + 8) = t;
            t |= (0x7F - *(unsigned char *)(e + 3)) >> 2;
            t |= 0x20;
            *(short *)(p + 8) = t;
        }
        goto scan;
    case 12:
        if (*(unsigned char *)(e + 3) >= 0x41) {
            *(short *)(e + 0x62) = 0x4000;
        } else {
            *(short *)(e + 0x62) = 0;
        }
        goto scan;
    case 15:
        func_00119810(0, *(unsigned char *)(e + 3));
        func_00119810(1, *(unsigned char *)(e + 3));
        break;
    case 16:
        {
            int c0 = *(unsigned char *)(e + 3);
            func_00119828(0, c0, c0);
        }
        {
            int c1 = *(unsigned char *)(e + 3);
            func_00119828(1, c1, c1);
        }
        break;
    case 17:
        func_00119858(0, *(unsigned char *)(e + 3));
        func_00119858(1, *(unsigned char *)(e + 3));
        break;
    case 18:
    case 19:
        func_00119840(0, *(unsigned char *)(e + 3));
        func_00119840(1, *(unsigned char *)(e + 3));
        break;
    default:
    scan:
        base = D_0027CCC0;
        g = D_00281AC0;
        i = 0;
        one = 1;
        r1 = base + 0xE;
        r3 = base + 6;
        do {
            t = *(unsigned short *)(r1 - 0xC);
            if (t == one
                && *(unsigned short *)(r1 - 0xA) == (*(unsigned char *)(e + 0) & 0xF)
                && *(unsigned short *)(r3 + 0x14) == t
                && *(unsigned short *)(r3 + 0) == *(int *)(e + 0x18)
                && (*(unsigned short *)(e + 0x60) == 0xFF
                    || *(unsigned short *)(r1 + 0) == *(unsigned short *)(e + 0x60))) {
                q = *(char **)(g + 4);
                func_001157F0(3, i, *(unsigned short *)(q + 6), *(unsigned short *)(q + 8));
            }
            i += 1;
            r1 += 0x6A;
            r3 += 0x6A;
        } while (i < 0x30);
        break;
    }
    *(int *)(e + 8) = *(int *)(e + 8) + 3;
}
