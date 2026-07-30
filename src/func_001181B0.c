// NEARMISS func_001181B0  (vram 0x001181B0, 0x264 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.84% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc loop strength-reduction (giv-splitting) wall + downstream GPR coloring. Body and control flow fully recovered — every compare, branch-likely, MMI-free op, both `mult rd,rs,rt` R5900 3-operand multiplies and both gcc div-by-zero traps reproduce. Residual: in the first loop the original gcc k...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

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
extern void func_001157F0(int a, int idx, int hi, int lo);

void func_001181B0(Event *ev)
{
    Voice *v;
    unsigned char *d;
    int t;
    int i;

    if (ev->f34 == 1) {
        for (i = 0; i < 48; i++) {
            v = &D_0027CCC0[i];
            if (v->f00 != 1) continue;
            if (v->f1A != 2) continue;
            if (v->f22 != ev->f24) continue;
            d = (unsigned char *)(ev->f08 + D_00281AC0.f14);
            if (v->f3E != d[4]) continue;
            if (v->f02 != d[5]) continue;
            if (v->f06 != ev->f18) continue;
            t = D_0027F740.f3A;
            v->f46 = 1;
            v->f50 = d[3];
            v->f52 = v->f2C;
            v->f54 = (d[2] << 2) * t / 60;
            t = D_0027F740.f3A;
            v->f56 = (d[2] << 2) * t / 60;
        }
        ev->f08 = ev->f08 + 6;
    } else {
        d = (unsigned char *)(ev->f08 + D_00281AC0.f14);
        D_00281AC0.f0C[3] = d[2];
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
