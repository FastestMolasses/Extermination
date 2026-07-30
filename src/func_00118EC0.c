// NEARMISS func_00118EC0  (vram 0x00118EC0, 0x1C0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 82.71% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc GPR-coloring wall + loop-iv/address-materialisation. Both loops are structurally recovered — same rotated do-while shapes, same `bnel` back-edge fills, same annulled `bnel v0,s8` skip, same store set, correct sizes (440 vs 448, 2 insns). Residuals: (a) loop 1 outer/inner counters are colour...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// Per-frame slot reaper. Loop 1: for each of 0x30 0x78-byte records, if it is
// active (f3E==1 && f32==1) and no D_0027CCC0 entry claims its index, clear the
// matching D_0027E0C0 record. Loop 2: age every D_0027CCC0 entry and reset the
// ones that have gone stale.
//
// All loop-2 addressing is written as `i * 0x6A` (rather than a hand-rolled
// offset/pointer accumulator) on purpose: that makes every address a giv of the
// single biv `i`, which is what stops gcc's check_dbra_loop from reversing the
// counter into a downcounter.

typedef struct Ent {            /* 0x6A bytes */
    unsigned char pad00[6];
    unsigned short f06;         /* 0x06 */
    unsigned char pad08[0x62];
} Ent;

extern void func_00121A28(void *dst, int c, int n);   /* memset */

extern Ent D_0027CCC0[];
extern unsigned char D_0027CCC8[];   /* = D_0027CCC0 + 0x08 */
extern unsigned char D_0027CCDA[];   /* = D_0027CCC0 + 0x1A */
extern unsigned char D_0027CCDC[];   /* = D_0027CCC0 + 0x1C */
extern unsigned char D_0027CCE2[];   /* = D_0027CCC0 + 0x22 */
extern unsigned char D_0027E0C0[];
extern int D_002817C0[];

void func_00118EC0(unsigned char *p)
{
    unsigned char *base;
    int i;
    int j;

    for (i = 0; i < 0x30; i++) {
        if (*(unsigned short *)(p + 0x3E) == 1 &&
            *(unsigned short *)(p + 0x32) == 1) {
            for (j = 0; j < 0x30; j++) {
                if (D_0027CCC0[j].f06 == i)
                    goto next;
            }
            func_00121A28(D_0027E0C0 + i * 0x78, 0, 0x78);
        }
    next:
        p += 0x78;
    }

    base = (unsigned char *)D_0027CCC0;
    for (i = 0; i < 0x30; i++) {
        if (D_002817C0[i] < 2 &&
            *(unsigned short *)(D_0027CCDC + i * 0x6A) >= 2 &&
            *(unsigned short *)(D_0027CCC8 + i * 0x6A) == 1 &&
            *(unsigned short *)(D_0027CCDA + i * 0x6A) != 3) {
            func_00121A28(base + i * 0x6A, 0, 0x6A);
            *(unsigned short *)(base + i * 0x6A + 6) = 0xFFFF;
            *(unsigned short *)(base + i * 0x6A + 0x26) = 0xFFFF;
            *(unsigned short *)(base + i * 0x6A + 0x24) = 0xFFFF;
            *(unsigned short *)(D_0027CCE2 + i * 0x6A) = 0xFFFF;
            *(unsigned short *)(base + i * 0x6A + 0x4E) = 0x78;
        }
        *(unsigned short *)(D_0027CCDC + i * 0x6A) += 1;
    }
}
