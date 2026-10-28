// NEARMISS func_00103DC8  (vram 0x00103DC8, 0x420 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 74.77% via ee-gcc 2.9-991111-01 (-O2) (was 51.59%). Logic, control flow, every store offset
// and the frame size (0xB0, five spill slots) match. Recovered (matching wave 2026-10-28): the slot
// table is a typed record (2 x 0x140-byte MCSlot then the active-slot index at +0x280); its address
// fields are pointers and the two per-entry tables hold function pointers, so type-based aliasing lets
// the active-slot index be read once at the end; the record pointers are taken straight from the
// array expression; the per-entry source base (buf + n * 0x600) is one local; the two parity bits are
// locals computed before the clamp test; the second half halves a4 in place.
// Remaining differences: global register allocation only (a8 lands in a temporary instead of $s5, the
// y-parity local in a saved register instead of $t9; the rest of the colouring follows), which no
// declaration-order or statement-order change moved.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// Motion-compensation entry builder (called from the macroblock motion decoder): appends one
// luma record (ra) and one chroma record (rb) to the active slot's entry n, stores the two
// reference-block pointers and the two copy-routine pointers chosen by the half-pel flags, and
// bumps the slot's entry count.
//
// COMPILER: eegcc
// CFLAGS: -O2

typedef struct {
    char *dst;
    int x;
    int w0;
    int w1;
    int step;
    char *src0;
    char *src1;
} MCRec;

typedef struct {
    char *buf;
    int pad4;
    char *ref0[4];
    char *ref1[4];
    void (*fn0[4])(void);
    void (*fn1[4])(void);
    MCRec ra[4];
    MCRec rb[4];
    int pad128;
    int count;
    int pad130[4];
} MCSlot;

typedef struct {
    MCSlot slot[2];
    int cur;
} MCState;

extern char *D_002412F0;
extern MCState D_002418F8;
extern void (*ftab_00241130[])(void);
extern void (*D_00241150[])(void);

void func_00103DC8(int *ctx, int a1, int a2, int a3, int a4, int a5, int a6, int a7,
                   int a8, int sh, int m)
{
    MCRec *ra;
    MCRec *rb;
    int n;
    char *d0;
    char *fb;
    int fx, fy, u, ub, ub2, v, u2, v2, q, q2, k, idx0, delta, flags1, flags2;

    n  = D_002418F8.slot[D_002418F8.cur].count;
    ra = &D_002418F8.slot[D_002418F8.cur].ra[n];
    rb = &D_002418F8.slot[D_002418F8.cur].rb[n];

    d0 = D_002412F0;

    u = (a7 >> 1) + a5;
    if (sh != 0)
        q = ((a8 >> 1) * 2 + a6) + (a3 + a1);
    else
        q = ((a8 >> 1) + a6) + (a3 + a1);

    ub = u >> 4;
    v = q >> 4;
    idx0 = ub * ctx[4] + v;
    ra->x = u - (ub << 4);
    ra->dst = d0 + ((a2 + a3) << 5);
    q -= v << 4;
    fx = a7 & 1;
    fy = a8 & 1;

    if (fy != 0) {
        if (q + (a4 << sh) >= 0x10) {
            k = (0x10 >> sh) - (q >> sh) - 1;
            ra->w0 = k;
            ra->w1 = a4 - k;
            goto ra_done;
        }
    } else {
        if (q + (a4 << sh) >= 0x11) {
            k = (0x10 >> sh) - (q >> sh);
            ra->w0 = k;
            ra->w1 = a4 - k;
            goto ra_done;
        }
    }
    ra->w0 = a4;
    ra->w1 = 0;
ra_done:

    fb = D_002418F8.slot[D_002418F8.cur].buf + n * 0x600;
    flags1 = (m << 2) | (fx << 1) | fy;
    ra->step = 0x10 << sh;
    ra->src0 = fb + (q << 4);
    ra->src1 = fb + ((q << 4) + 0x300);

    a7 = a7 / 2;
    a4 = a4 >> 1;
    a8 = a8 / 2;

    u2 = (a7 >> 1) + (a5 >> 1);
    if (sh != 0)
        q2 = ((a8 >> 1) * 2 + (a6 >> 1)) + ((a3 >> 1) + a1);
    else
        q2 = ((a8 >> 1) + (a6 >> 1)) + ((a3 >> 1) + a1);

    ub2 = u2 >> 3;
    v2 = q2 >> 3;
    rb->x = u2 - (ub2 << 3);
    rb->dst = d0 + (((a2 + (a3 >> 1)) << 4) + 0x200);
    q2 -= v2 << 3;
    fx = a7 & 1;
    fy = a8 & 1;

    if (fy != 0) {
        if (q2 + (a4 << sh) >= 0x8) {
            k = (0x8 >> sh) - (q2 >> sh) - 1;
            rb->w0 = k;
            rb->w1 = a4 - k;
            goto rb_done;
        }
    } else {
        if (q2 + (a4 << sh) >= 0x9) {
            k = (0x8 >> sh) - (q2 >> sh);
            rb->w0 = k;
            rb->w1 = a4 - k;
            goto rb_done;
        }
    }
    rb->w0 = a4;
    rb->w1 = 0;
rb_done:
    rb->step = 0x8 << sh;

    delta = (ub2 - ub) * 2 + (v2 - v);
    flags2 = (m << 2) | (fx << 1) | fy;

    rb->src0 = (delta * 0x180 + fb) + ((q2 << 3) + 0x100);
    rb->src1 = (delta * 0x180 + fb) + ((q2 << 3) + 0x400);
    D_002418F8.slot[D_002418F8.cur].ref0[n] = (char *)ctx[0] + idx0 * 0x180;
    D_002418F8.slot[D_002418F8.cur].ref1[n] = (char *)ctx[0] + (idx0 + ctx[4]) * 0x180;
    D_002418F8.slot[D_002418F8.cur].fn0[n] = ftab_00241130[flags1];
    D_002418F8.slot[D_002418F8.cur].fn1[n] = D_00241150[flags2];
    D_002418F8.slot[D_002418F8.cur].count++;
}
