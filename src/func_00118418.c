// COMPILER: eegcc
// CFLAGS: -O2

/*
 * Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 76.92% NEARMISS).
 * Fixes read from the original instructions:
 * - Written in the spelling of the byte-matched twin func_001181B0: typed Voice /
 *   Event / SeqGlobals / TempoBlock records, the voice table indexed directly
 *   (D_0027CCC0[i].field) in plain for (i < 48) loops with continue guards, the
 *   event data pointer re-formed from ev->f08 + D_00281AC0.f14 at each use. The
 *   old hand-made byte pointer walk was what looked like a strength-reduction wall.
 * - func_00117BA0 is declared returning int here: the original shifts its result
 *   arithmetically (its definition in src/ returns unsigned char, which compiles
 *   to a logical shift and costs the match).
 * - The function returns the new cursor through `return ev->f08 = ev->f08 + k;`.
 *
 * Sequencer command handler, dispatched on ev->f34.
 *   f34 == 1: for every voice with f00 == 1, f1A == 2, f22 == ev->f24 whose
 *             f3E / f02 equal data bytes 4 / 5 and f06 == ev->f18: set f48 = 1,
 *             f58 = data[3], f5A = f4C, and f5C = f5E = (data[2] << 2) * tempo / 60
 *             (tempo = D_0027F740.f3A). Advances ev->f08 by 6.
 *   otherwise: copy data[2] to D_00281AC0.f0C[4]; for every voice whose f04 ==
 *             (ev->f00 & 0xF), f22 == ev->f24, f06 == ev->f18, f08 != 1, f1A == 1
 *             and f00 == 1: f4C = data[2], f32 = D_00242630[func_00117BA0(0, f0E) >> 2],
 *             then pass func_001179E0's packed (hi << 16 | lo) result to
 *             func_001157F0(1, i, hi, lo). Advances ev->f08 by 3.
 * "data" is the event's byte record at ev->f08 + D_00281AC0.f14. Returns the new
 * ev->f08.
 */

typedef struct {
    unsigned short f00;      /* 0x00 */
    unsigned short f02;      /* 0x02 */
    unsigned short f04;      /* 0x04 */
    unsigned short f06;      /* 0x06 */
    unsigned short f08;      /* 0x08 */
    char pad_0A[0x04];
    unsigned short f0E;      /* 0x0E */
    char pad_10[0x0A];
    unsigned short f1A;      /* 0x1A */
    char pad_1C[0x06];
    unsigned short f22;      /* 0x22 */
    char pad_24[0x0E];
    unsigned short f32;      /* 0x32 */
    char pad_34[0x0A];
    unsigned short f3E;      /* 0x3E */
    char pad_40[0x08];
    unsigned short f48;      /* 0x48 */
    char pad_4A[0x02];
    unsigned short f4C;      /* 0x4C */
    char pad_4E[0x0A];
    unsigned short f58;      /* 0x58 */
    unsigned short f5A;      /* 0x5A */
    unsigned short f5C;      /* 0x5C */
    unsigned short f5E;      /* 0x5E */
    char pad_60[0x0A];
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
extern unsigned short D_00242630[];

extern int func_00117BA0(int a0, int a1);
extern int func_001179E0(int idx, Event *ev);
extern int func_001157F0(int a, int idx, int hi, int lo);

int func_00118418(Event *ev)
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
            D_0027CCC0[i].f48 = 1;
            D_0027CCC0[i].f58 = d[3];
            D_0027CCC0[i].f5A = D_0027CCC0[i].f4C;
            D_0027CCC0[i].f5C = (d[2] << 2) * D_0027F740.f3A / 60;
            D_0027CCC0[i].f5E = (d[2] << 2) * D_0027F740.f3A / 60;
        }
        return ev->f08 = ev->f08 + 6;
    } else {
        D_00281AC0.f0C[4] = ((unsigned char *)(ev->f08 + D_00281AC0.f14))[2];
        for (i = 0; i < 48; i++) {
            int r;
            if (D_0027CCC0[i].f04 != (ev->f00 & 0xF)) continue;
            if (D_0027CCC0[i].f22 != ev->f24) continue;
            if (D_0027CCC0[i].f06 != ev->f18) continue;
            if (D_0027CCC0[i].f08 == 1) continue;
            if (D_0027CCC0[i].f1A != 1) continue;
            if (D_0027CCC0[i].f00 != 1) continue;
            D_0027CCC0[i].f4C = ((unsigned char *)(ev->f08 + D_00281AC0.f14))[2];
            D_0027CCC0[i].f32 = D_00242630[func_00117BA0(0, D_0027CCC0[i].f0E) >> 2];
            r = func_001179E0(i, ev);
            func_001157F0(1, i, r >> 16, r & 0xFFFF);
        }
        return ev->f08 = ev->f08 + 3;
    }
}
