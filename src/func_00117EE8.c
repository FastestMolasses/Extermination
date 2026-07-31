// COMPILER: eegcc
// CFLAGS: -O2
// Sequencer tempo event: derive the tick divisor from the event payload and
// apply it to every voice slot that matches the running event's channel/key.

typedef struct {                /* 0x6A */
    unsigned short f00;         /* 0x00 */
    unsigned short f02;         /* 0x02 */
    unsigned short f04;         /* 0x04 */
    unsigned short f06;         /* 0x06 */
    unsigned char pad08[0x1A - 0x08];
    unsigned short f1A;         /* 0x1A */
    unsigned char pad1C[0x22 - 0x1C];
    unsigned short f22;         /* 0x22 */
    unsigned char pad24[0x3C - 0x24];
    unsigned short f3C;         /* 0x3C */
    unsigned short f3E;         /* 0x3E */
    unsigned char pad40[0x6A - 0x40];
} Voice;

typedef struct {
    unsigned char f00;          /* 0x00 */
    unsigned char pad01[0x08 - 0x01];
    int f08;                    /* 0x08 */
    unsigned char pad0C[0x18 - 0x0C];
    int f18;                    /* 0x18 */
    unsigned char pad1C[0x24 - 0x1C];
    unsigned short f24;         /* 0x24 */
    unsigned char pad26[0x34 - 0x26];
    unsigned short f34;         /* 0x34 */
} Seq;

typedef struct {
    int f00;                    /* 0x00 */
    int f04;                    /* 0x04 */
    int f08;                    /* 0x08 */
    unsigned char *p0C;         /* 0x0C */
    int f10;                    /* 0x10 */
    unsigned char *p14;         /* 0x14 */
} Song;

extern Voice D_0027CCC0[48];
extern Song D_00281AC0;

void func_00117EE8(Seq *seq)
{
    int i;
    int q;

    q = 240 / (60 - D_00281AC0.p14[seq->f08 + 2] * 58 / 127);
    if (seq->f34 == 1) {
        for (i = 0; i < 48; i++) {
            if (D_0027CCC0[i].f04 != (seq->f00 & 0xF)) continue;
            if (D_0027CCC0[i].f3E != D_00281AC0.p14[seq->f08 + 3]) continue;
            if (D_0027CCC0[i].f02 != D_00281AC0.p14[seq->f08 + 4]) continue;
            if (D_0027CCC0[i].f22 != seq->f24) continue;
            if (D_0027CCC0[i].f06 != seq->f18) continue;
            if (D_0027CCC0[i].f1A != 2) continue;
            if (D_0027CCC0[i].f00 == 1) D_0027CCC0[i].f3C = q;
        }
        seq->f08 += 5;
    } else {
        D_00281AC0.p0C[12] = q;
        for (i = 0; i < 48; i++) {
            if (D_0027CCC0[i].f04 != (seq->f00 & 0xF)) continue;
            if (D_0027CCC0[i].f22 != seq->f24) continue;
            if (D_0027CCC0[i].f06 != seq->f18) continue;
            if (D_0027CCC0[i].f1A != 1) continue;
            if (D_0027CCC0[i].f00 == 1) D_0027CCC0[i].f3C = q;
        }
        seq->f08 += 3;
    }
}
