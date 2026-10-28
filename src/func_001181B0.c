// COMPILER: eegcc
// CFLAGS: -O2

/*
 * Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 93.84% NEARMISS).
 * Fixes read from the original instructions:
 * - The first loop indexes the voice table directly (D_0027CCC0[i].field) instead of a
 *   cached element pointer; that gives gcc's loop strength reduction the original's
 *   separate address giv and keeps the tempo word re-read for the second duration.
 * - The else branch copies the event's byte straight from the sequence data
 *   without a named data pointer, which restores the original register colouring.
 * - func_001157F0 is declared returning int, as its definition in src/ does.
 *
 * Event handler over the 48-entry voice table: when ev->f34 == 1 it updates every
 * voice with f00 == 1 and f1A == 2 whose fields match the event and its 6-byte data
 * record (two values scaled by D_0027F740.f3A / 60) and advances ev->f08 by 6;
 * otherwise it copies data byte 2 to D_00281AC0.f0C[3], calls func_001179E0 /
 * func_001157F0 for every matching voice with f1A == 1, and advances ev->f08 by 3.
 */

typedef struct {
    unsigned short f00;      /* 0x00 */
    unsigned short f02;      /* 0x02 */
    unsigned short f04;      /* 0x04 */
    unsigned short f06;      /* 0x06 */
    unsigned short f08;      /* 0x08 */
    char pad_0A[0x10];
    unsigned short f1A;      /* 0x1A */
    char pad_1C[0x06];
    unsigned short f22;      /* 0x22 */
    char pad_24[0x08];
    unsigned short f2C;      /* 0x2C */
    char pad_2E[0x06];
    unsigned short f34;      /* 0x34 */
    char pad_36[0x08];
    unsigned short f3E;      /* 0x3E */
    char pad_40[0x06];
    unsigned short f46;      /* 0x46 */
    char pad_48[0x08];
    unsigned short f50;      /* 0x50 */
    unsigned short f52;      /* 0x52 */
    unsigned short f54;      /* 0x54 */
    unsigned short f56;      /* 0x56 */
    char pad_58[0x12];
} Voice;                     /* 0x6A */

typedef struct {
    char pad_00[0x0C];
    unsigned char *f0C;      /* 0x0C */
    char pad_10[0x04];
    int f14;                 /* 0x14 */
} SeqGlobals;

typedef struct {
    char pad_00[0x3A];
    unsigned short f3A;      /* 0x3A */
} TempoBlock;

typedef struct {
    unsigned char f00;       /* 0x00 */
    char pad_01[0x07];
    int f08;                 /* 0x08 */
    char pad_0C[0x0C];
    int f18;                 /* 0x18 */
    char pad_1C[0x08];
    unsigned short f24;      /* 0x24 */
    char pad_26[0x0E];
    unsigned short f34;      /* 0x34 */
} Event;

extern Voice D_0027CCC0[48];
extern SeqGlobals D_00281AC0;
extern TempoBlock D_0027F740;

extern int func_001179E0(int idx, Event *ev);
extern int func_001157F0(int a, int idx, int hi, int lo);

void func_001181B0(Event *ev)
{
    unsigned char *d;
    int i;

    if (ev->f34 == 1) {
        for (i = 0; i < 48; i++) {
            if (D_0027CCC0[i].f00 != 1) continue;
            if (D_0027CCC0[i].f1A != 2) continue;
            if (D_0027CCC0[i].f22 != ev->f24) continue;
            d = (unsigned char *)(ev->f08 + D_00281AC0.f14);
            if (D_0027CCC0[i].f3E != d[4]) continue;
            if (D_0027CCC0[i].f02 != d[5]) continue;
            if (D_0027CCC0[i].f06 != ev->f18) continue;
            D_0027CCC0[i].f46 = 1;
            D_0027CCC0[i].f50 = d[3];
            D_0027CCC0[i].f52 = D_0027CCC0[i].f2C;
            D_0027CCC0[i].f54 = (d[2] << 2) * D_0027F740.f3A / 60;
            D_0027CCC0[i].f56 = (d[2] << 2) * D_0027F740.f3A / 60;
        }
        ev->f08 = ev->f08 + 6;
    } else {
        D_00281AC0.f0C[3] = ((unsigned char *)(ev->f08 + D_00281AC0.f14))[2];
        for (i = 0; i < 48; i++) {
            int r;
            if (D_0027CCC0[i].f00 != 1) continue;
            if (D_0027CCC0[i].f1A != 1) continue;
            if (D_0027CCC0[i].f04 != (ev->f00 & 0xF)) continue;
            if (D_0027CCC0[i].f22 != ev->f24) continue;
            if (D_0027CCC0[i].f08 == 1) continue;
            if (D_0027CCC0[i].f06 != ev->f18) continue;
            D_0027CCC0[i].f34 = D_00281AC0.f0C[3];
            r = func_001179E0(i, ev);
            func_001157F0(1, i, r >> 16, r & 0xFFFF);
        }
        ev->f08 = ev->f08 + 3;
    }
}
