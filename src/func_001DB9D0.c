// NEARMISS func_001DB9D0  (vram 0x001DB9D0, 0x2D4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.08% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful.
// Remaining differences: register colouring only. Every opcode, offset and constant of the
// header and loop matches, and the rotation seed's multiply-accumulate sequence now matches;
// the original keeps the ring radius term in $f24 (this C: $f21, which costs two nops before
// the divide) and allocates the packet-header temporaries two registers higher (a0/a1 stay
// unused), which survived declaration-order, statement-order and inline-helper trials.
// Wave 2026-10-27 fixes: the start vector is (denom * sin 0, denom * cos 0) (x from the sine
// call, y from the cosine; the old C had them swapped, which also hid the multiply-add
// fusion); D_00275670 is a gp-relative pointer (-sdatathreshold 4) and the header uses the
// packet-family slot spelling with a 128-bit zero store and 64-bit GIF words (twin
// func_001D6580); D_00253250..D_0025327C are one parameter block (centre, radius, two
// vertex templates) read absolutely; the loop counter is declared before the cursor.
//
// SEMANTICS: draws a 33-step ring as a GIF strip: the radius 768 - 568 * dt is rotated by
// 0.09817477 rad (pi / 32) per step with a second-order recurrence (x -= k * nx,
// nx += k * x, k = 2 cos step); each step emits two vertices, the inner one scaled by
// radius / denom about the centre (0.8 x, 0.5 y) and the outer one unscaled, in 12.4
// fixed point via float_to_int, with the colour/template words from the parameter block.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

typedef unsigned __int128 u128;

struct ring_params {
    float cx;          /* 0x00253250 */
    float cy;          /* 0x00253254 */
    float unk08;       /* 0x00253258 */
    float radius;      /* 0x0025325C */
    int inner[4];      /* 0x00253260 */
    int outer[4];      /* 0x00253270 */
};

extern int float_to_int(float);
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern void func_001D1F80(int, int, int);
extern struct ring_params D_00253250;
extern char **D_00275670;

void func_001DB9D0(int arg0, float dt) {
    float denom;
    float c0;
    float s0;
    float cStep;
    float sStep;
    float scale;
    float k;
    float x;
    float y;
    float nx;
    float ny;
    float radius;
    char **slot;
    char *t;
    int i;
    char *p;

    denom = 768.0f + (-568.0f * dt);
    radius = D_00253250.radius;
    c0 = func_0011E2A8(0.0f);
    s0 = func_0011DE90(0.0f);
    cStep = func_0011E2A8(0.09817477f);
    sStep = func_0011DE90(0.09817477f);
    scale = radius / denom;
    x = denom * s0;
    y = denom * c0;
    k = 2.0f * cStep;
    nx = x * cStep + sStep * y;
    ny = y * cStep - sStep * x;
    func_001D1F80(arg0, 0, 1);

    slot = (char **)(&D_00275670[arg0]);
    *((char *)(slot[4] + 3)) = 0x10;
    *((int *)(slot[4] + 4)) = 0;
    *((short *)(slot[4] + 0)) = 0x86;
    t = slot[4];
    p = t + 0x10;
    slot[4] = t + 0x870;
    *((u128 *)p) = 0;
    *((int *)(t + 0x1C)) = 0x50000085;
    *((long long *)(t + 0x20)) = 0x8021 | (((long long)0x40264000) << 0x20);
    *((long long *)(t + 0x28)) = 0x4141;
    p += 0x20;

    i = 0;
    do {
        *(int *)(p + 0x00) = D_00253250.inner[0];
        *(int *)(p + 0x04) = D_00253250.inner[1];
        *(int *)(p + 0x08) = D_00253250.inner[2];
        *(int *)(p + 0x0C) = D_00253250.inner[3];
        *(int *)(p + 0x20) = D_00253250.outer[0];
        *(int *)(p + 0x24) = D_00253250.outer[1];
        *(int *)(p + 0x28) = D_00253250.outer[2];
        *(int *)(p + 0x2C) = D_00253250.outer[3];
        *(int *)(p + 0x10) = float_to_int(16.0f * (D_00253250.cx + (0.8f * (x * scale))));
        *(int *)(p + 0x14) = float_to_int(16.0f * (D_00253250.cy + (0.5f * (y * scale))));
        *(int *)(p + 0x18) = 0xFFFFFF;
        *(int *)(p + 0x1C) = 0;
        *(int *)(p + 0x30) = float_to_int(16.0f * ((0.8f * x) + D_00253250.cx));
        *(int *)(p + 0x34) = float_to_int(16.0f * ((0.5f * y) + D_00253250.cy));
        *(int *)(p + 0x38) = 0xFFFFFF;
        x -= k * nx;
        i += 1;
        *(int *)(p + 0x3C) = 0;
        p += 0x40;
        y -= k * ny;
        nx += k * x;
        ny += k * y;
    } while (i < 0x21);
}
