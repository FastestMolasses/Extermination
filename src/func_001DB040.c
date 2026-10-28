// NEARMISS func_001DB040  (vram 0x001DB040, 0x200 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.41% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) (was an asm-word body, matching
// wave 2026-10-28). Damping, spring and 4 + stiffness are hoisted into locals; velocity uses compound *= / +=; loop counters declared j, i.
// Remaining differences: register colouring only, in the final integration loop: the
// original keeps the column counter in a0 and the cell pointer in a2 (ours swapped), and
// the clamped height in f2 (ours f1 or f2 depending on spelling). Tried: fresh loop
// variables, a row-pointer walk, ternary clamp, swapped declaration orders; the permuter
// (400 s, 2 jobs) plateaued at its base score.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Steps the 32x32 water / heat-haze height grid (struct grid, also used by
// func_001DB480). Per cell, with clamped neighbours (edges reuse the edge row or
// column): lap = up + (down + (left + right)) - (4 + stiff) * h; vel = vel * damp;
// vel += lap * k; u = 2 * (left - right); v = 2 * (down - up), where 'down' is row
// i - 1. Then one random cell (row and column = (rand() >> 16) % 32 each, two
// func_00122BB8 calls) gets vel += impulse (+0x400C). Finally every height
// integrates h += vel, clamped to at most 70.0.
struct cell {
    float u;        /* 0x0 slope along the row (written here) */
    float v;        /* 0x4 slope along the column */
    float vel;      /* 0x8 height velocity */
    float h;        /* 0xC height */
};

struct grid {
    struct cell c[32][32];
    float damp;     /* 0x4000 */
    float k;        /* 0x4004 */
    float stiff;    /* 0x4008 */
    float impulse;  /* 0x400C */
};

extern int func_00122BB8(void);

void func_001DB040(struct grid *g) {
    int j, i;
    int im, ip, jm, jp;
    int r, c;
    float lap, h;
    float damp, k, stiff;

    stiff = 4.0f + g->stiff;
    damp = g->damp;
    k = g->k;
    for (i = 0; i < 32; i++) {
        im = i - 1;
        ip = i + 1;
        if (im < 0) {
            im = 0;
        }
        if (ip >= 32) {
            ip = 31;
        }
        for (j = 0; j < 32; j++) {
            jm = j - 1;
            jp = j + 1;
            if (jm < 0) {
                jm = 0;
            }
            if (jp >= 32) {
                jp = 31;
            }
            lap = g->c[ip][j].h + (g->c[im][j].h + (g->c[i][jm].h + g->c[i][jp].h))
                - stiff * g->c[i][j].h;
            g->c[i][j].vel *= damp;
            g->c[i][j].vel += lap * k;
            g->c[i][j].u = 2.0f * (g->c[i][jm].h - g->c[i][jp].h);
            g->c[i][j].v = 2.0f * (g->c[im][j].h - g->c[ip][j].h);
        }
    }

    r = (func_00122BB8() >> 16) % 32;
    c = (func_00122BB8() >> 16) % 32;
    g->c[r][c].vel += g->impulse;

    for (i = 0; i < 32; i++) {
        for (j = 0; j < 32; j++) {
            h = g->c[i][j].h;
            h += g->c[i][j].vel;
            if (h > 70.0f) {
                h = 70.0f;
            }
            g->c[i][j].h = h;
        }
    }
}
