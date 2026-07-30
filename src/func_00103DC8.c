// NEARMISS func_00103DC8  (vram 0x00103DC8, 0x420 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 51.59% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc register-allocation + list-scheduler wall. The control flow, every arithmetic expression, every store offset and every branch-likely site are recovered (all four `bnel`-guarded clamp blocks, both `sh`-conditional duplicated add chains, the three separate re-reads of the active-slot index, t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_002412F0;
extern char D_002418F8[];     /* 2 x 0x140-byte slots, then the active-slot index at +0x280 */
extern int ftab_00241130[];
extern int D_00241150[];

void func_00103DC8(int *ctx, int a1, int a2, int a3, int a4, int a5, int a6, int a7,
                   int a8, int sh, int m)
{
    char *base;
    char *slot;
    char *ra;
    char *rb;
    int n, cur, d0, f0;
    int u, v, u2, v2, q, q2, k, idx0, delta, flags1, flags2, h4;

    base = D_002418F8;
    cur = *(int *)(base + 0x280) * 0x140;
    n  = *(int *)(base + cur + 0x12C);
    ra = cur + (n * 0x1C + (base + 0x48));
    rb = cur + (n * 0x1C + (base + 0xB8));

    d0 = D_002412F0;

    u = (a7 >> 1) + a5;
    if (sh != 0)
        q = ((a8 >> 1) * 2 + a6) + (a3 + a1);
    else
        q = ((a8 >> 1) + a6) + (a3 + a1);

    v = q >> 4;
    idx0 = (u >> 4) * ctx[4] + v;
    *(int *)(ra + 0x04) = u - ((u >> 4) << 4);
    *(int *)(ra + 0x00) = d0 + ((a2 + a3) << 5);
    q -= v << 4;

    if ((a8 & 1) != 0) {
        if (q + (a4 << sh) >= 0x10) {
            k = (0x10 >> sh) - (q >> sh) - 1;
            *(int *)(ra + 0x08) = k;
            *(int *)(ra + 0x0C) = a4 - k;
            goto ra_done;
        }
    } else {
        if (q + (a4 << sh) >= 0x11) {
            k = (0x10 >> sh) - (q >> sh);
            *(int *)(ra + 0x08) = k;
            *(int *)(ra + 0x0C) = a4 - k;
            goto ra_done;
        }
    }
    *(int *)(ra + 0x08) = a4;
    *(int *)(ra + 0x0C) = 0;
ra_done:

    slot = base + *(int *)(base + 0x280) * 0x140;
    f0 = *(int *)(slot + 0x00);
    flags1 = (m << 2) | ((a7 & 1) << 1) | (a8 & 1);
    *(int *)(ra + 0x10) = 0x10 << sh;
    *(int *)(ra + 0x14) = (f0 + n * 0x600) + (q << 4);
    *(int *)(ra + 0x18) = (f0 + n * 0x600) + ((q << 4) + 0x300);

    a7 = a7 / 2;
    a8 = a8 / 2;

    u2 = (a7 >> 1) + (a5 >> 1);
    if (sh != 0)
        q2 = ((a8 >> 1) * 2 + (a6 >> 1)) + ((a3 >> 1) + a1);
    else
        q2 = ((a8 >> 1) + (a6 >> 1)) + ((a3 >> 1) + a1);

    h4 = a4 >> 1;
    v2 = q2 >> 3;
    *(int *)(rb + 0x04) = u2 - ((u2 >> 3) << 3);
    *(int *)(rb + 0x00) = d0 + (((a2 + (a3 >> 1)) << 4) + 0x200);
    q2 -= v2 << 3;

    if ((a8 & 1) != 0) {
        if (q2 + (h4 << sh) >= 0x8) {
            k = (0x8 >> sh) - (q2 >> sh) - 1;
            *(int *)(rb + 0x08) = k;
            *(int *)(rb + 0x0C) = h4 - k;
            goto rb_done;
        }
    } else {
        if (q2 + (h4 << sh) >= 0x9) {
            k = (0x8 >> sh) - (q2 >> sh);
            *(int *)(rb + 0x08) = k;
            *(int *)(rb + 0x0C) = h4 - k;
            goto rb_done;
        }
    }
    *(int *)(rb + 0x08) = h4;
    *(int *)(rb + 0x0C) = 0;
rb_done:
    *(int *)(rb + 0x10) = 0x8 << sh;

    delta = ((u2 >> 3) - (u >> 4)) * 2 + (v2 - v);
    flags2 = (m << 2) | ((a7 & 1) << 1) | (a8 & 1);

    slot = base + *(int *)(base + 0x280) * 0x140;
    *(int *)(rb + 0x14) = (delta * 0x180 + (f0 + n * 0x600)) + ((q2 << 3) + 0x100);
    *(int *)(rb + 0x18) = (delta * 0x180 + (f0 + n * 0x600)) + ((q2 << 3) + 0x400);
    *(int *)(slot + 0x08 + n * 4) = ctx[0] + idx0 * 0x180;
    *(int *)(slot + 0x18 + n * 4) = ctx[0] + (idx0 + ctx[4]) * 0x180;
    *(int *)(slot + 0x28 + n * 4) = ftab_00241130[flags1];
    *(int *)(slot + 0x38 + n * 4) = D_00241150[flags2];
    *(int *)(slot + 0x12C) = *(int *)(slot + 0x12C) + 1;
}
