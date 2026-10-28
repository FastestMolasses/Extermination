// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 82.71% NEARMISS).
// Fixes read from the original instructions:
//  - Typed records indexed directly (Voice D_0027CCC0[48] as in the byte-matched
//    voice siblings, a 0x78-byte Chan record for D_0027E0C0 and the argument)
//    instead of byte offsets from split field symbols.
//  - The second loop reuses the inner-loop counter j (not i): that one variable
//    lives across the second loop's call, which puts it in a saved register and
//    leaves i in an argument register, as in the original.
//  - The four 0xFFFF stores are one chained assignment (f06 first, then f26, f24,
//    f22), which gives the original's store order and address forms.
//
// Per-frame slot reaper. Loop 1: for each of the 48 0x78-byte channel records,
// if it is active (f3E == 1 && f32 == 1) and no voice in D_0027CCC0 claims its
// index (voice f06), clear the matching D_0027E0C0 record. Loop 2: for every
// voice whose D_002817C0 word is below 2, whose age f1C is at least 2, with
// f08 == 1 and f1A != 3, clear the voice and reset f06/f22/f24/f26 to 0xFFFF
// and f4E to 0x78; then age every voice (f1C += 1).

typedef struct {                /* 0x6A */
    unsigned short f00;
    unsigned short f02;
    unsigned short f04;
    unsigned short f06;         /* 0x06 */
    unsigned short f08;         /* 0x08 */
    unsigned char pad0A[0x1A - 0x0A];
    unsigned short f1A;         /* 0x1A */
    unsigned short f1C;         /* 0x1C */
    unsigned char pad1E[0x22 - 0x1E];
    unsigned short f22;         /* 0x22 */
    unsigned short f24;         /* 0x24 */
    unsigned short f26;         /* 0x26 */
    unsigned char pad28[0x4E - 0x28];
    unsigned short f4E;         /* 0x4E */
    unsigned char pad50[0x6A - 0x50];
} Voice;

typedef struct {                /* 0x78 */
    unsigned char pad00[0x32];
    unsigned short f32;
    unsigned char pad34[0x3E - 0x34];
    unsigned short f3E;
    unsigned char pad40[0x78 - 0x40];
} Chan;

extern void func_00121A28(void *dst, int c, int n);

extern Voice D_0027CCC0[48];
extern Chan D_0027E0C0[48];
extern int D_002817C0[];

void func_00118EC0(Chan *p)
{
    int i;
    int j;

    for (i = 0; i < 48; i++, p++) {
        if (p->f3E == 1 && p->f32 == 1) {
            for (j = 0; j < 48; j++) {
                if (D_0027CCC0[j].f06 == i)
                    goto next;
            }
            func_00121A28(&D_0027E0C0[i], 0, 0x78);
        }
    next:
        ;
    }

    for (j = 0; j < 48; j++) {
        if (D_002817C0[j] < 2 && D_0027CCC0[j].f1C >= 2 &&
            D_0027CCC0[j].f08 == 1 && D_0027CCC0[j].f1A != 3) {
            func_00121A28(&D_0027CCC0[j], 0, 0x6A);
            D_0027CCC0[j].f22 = D_0027CCC0[j].f24 = D_0027CCC0[j].f26 = D_0027CCC0[j].f06 = 0xFFFF;
            D_0027CCC0[j].f4E = 0x78;
        }
        D_0027CCC0[j].f1C++;
    }
}
