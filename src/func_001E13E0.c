// NEARMISS func_001E13E0  (vram 0x001E13E0, 0x37C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.10% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful.
// Fixes read from the original (matching wave 2026-10-27, was 81.71%): D_00275670 is gp-relative
// (-sdatathreshold 8); the quadword count is an unsigned local (0x68) set before the loop, so
// the DMA size (qwc + 1) * 16 and the DIRECT word (qwc - 1) | 0x50000000 stay unfolded; the
// header is written through q = p + 0x10 with a quadword zero store; the next-row index is
// computed right after the channel pointer; the vertex table is a [16][16] array of 0x18-byte
// records; i++ and the row advance sit at the end of the outer loop; declaration order.
// Remaining differences: one instruction -- the original copies the next-row pointer into the
// inner-loop cursor with a plain integer move placed before the j/a initialisations, the build
// uses the 128-bit copy form after them (same register, same size 0x37C).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Builds a per-frame GIFtag sprite-grid packet for the 16x16 water surface
// table at D_0081E0F0 (16 rows of 16 records of 0x18 bytes: x, y at +0/+4,
// u, v at +0x10/+0x14). Indexes the channel table at D_00275670+arg0*4; the
// entry's +0x10 field is a bump-allocator cursor pointer advanced by
// (qwc + 1) * 16 = 0x690 per row after a DMA tag header (byte +3 = 0x10,
// word +4 = 0, half +0 = qwc 0x68). Each row zeroes the quadword at +0x10,
// writes VIF DIRECT(qwc - 1) at +0x1C, a GIFtag (REGLIST 0x600A4000.../
// 0x512512), then 16 vertex pairs (rows i and i+1, wrapping) of 0x60 bytes
// plus a wrap record whose u is 1.0 + row start's u and x is fixed at 0x8800.
// float_to_int converts the 30720 + 16*coord screen positions; row 15 adds a
// 1.0 v bias and a 0x1000 y bias to the second row of each pair.
typedef unsigned int u128 __attribute__((mode(TI)));

typedef struct {
    float x;
    float y;
    float z;
    float w;
    float u;
    float v;
} WaterVtx;

extern char *D_00275670;
extern WaterVtx D_0081E0F0[16][16];
extern int float_to_int(float);

void func_001E13E0(int arg0) {
    int j;
    int bias;
    char *q;
    WaterVtx *a;
    WaterVtx *b;
    WaterVtx *row;
    int i;
    WaterVtx *next;
    char *ch;
    char *p;
    int k;
    unsigned int qwc;
    unsigned int sz;
    float vbias;

    qwc = 0x68;
    sz = (qwc + 1) * 16;
    row = D_0081E0F0[0];
    i = 0;
    do {
        ch = D_00275670 + arg0 * 4;
        k = (i + 1) & 0xF;
        *(char *)(*(char **)(ch + 0x10) + 3) = 0x10;
        *(int *)(*(char **)(ch + 0x10) + 4) = 0;
        *(short *)(*(char **)(ch + 0x10) + 0) = qwc;
        p = *(char **)(ch + 0x10);
        *(char **)(ch + 0x10) = p + sz;
        q = p + 0x10;

        if (i == 0xF) {
            vbias = 1.0f;
            bias = 0x1000;
        } else {
            vbias = 0.0f;
            bias = 0;
        }

        *(u128 *)q = 0;
        *(int *)(q + 0xC) = (qwc - 1) | 0x50000000;
        *(long long *)(q + 0x10) = (long long)0x8011 | ((long long)0x600A4000 << 32);
        *(long long *)(q + 0x18) = 0x512512;

        next = D_0081E0F0[k];
        q += 0x20;
        for (j = 0, a = row, b = next; j < 16; j++, a++, b++) {
            *(float *)(q + 0x0) = a->u;
            *(float *)(q + 0x4) = a->v;
            *(int *)(q + 0x8) = 0x3F800000;
            *(int *)(q + 0x10) = 0x40;
            *(int *)(q + 0x14) = 0x40;
            *(int *)(q + 0x18) = 0x40;
            *(int *)(q + 0x1C) = 0x80;
            *(int *)(q + 0x20) = float_to_int(30720.0f + (16.0f * a->x));
            *(int *)(q + 0x24) = float_to_int(30720.0f + (16.0f * a->y));
            *(int *)(q + 0x28) = 0;
            *(int *)(q + 0x2C) = 0;
            *(float *)(q + 0x30) = b->u;
            *(float *)(q + 0x34) = vbias + b->v;
            *(int *)(q + 0x38) = 0x3F800000;
            *(int *)(q + 0x40) = 0x40;
            *(int *)(q + 0x44) = 0x40;
            *(int *)(q + 0x48) = 0x40;
            *(int *)(q + 0x4C) = 0x80;
            *(int *)(q + 0x50) = float_to_int(30720.0f + (16.0f * b->x));
            *(int *)(q + 0x54) = float_to_int((float)bias + (30720.0f + (16.0f * b->y)));
            *(int *)(q + 0x58) = 0;
            *(int *)(q + 0x5C) = 0;
            q += 0x60;
        }

        *(float *)(q + 0x0) = 1.0f + row->u;
        *(float *)(q + 0x4) = row->v;
        *(int *)(q + 0x8) = 0x3F800000;
        *(int *)(q + 0x10) = 0x40;
        *(int *)(q + 0x14) = 0x40;
        *(int *)(q + 0x18) = 0x40;
        *(int *)(q + 0x1C) = 0x80;
        *(int *)(q + 0x20) = 0x8800;
        *(int *)(q + 0x24) = float_to_int(30720.0f + (16.0f * row->y));
        *(int *)(q + 0x28) = 0;
        *(int *)(q + 0x2C) = 0;
        *(float *)(q + 0x30) = 1.0f + next->u;
        *(float *)(q + 0x34) = vbias + next->v;
        *(int *)(q + 0x38) = 0x3F800000;
        *(int *)(q + 0x40) = 0x40;
        *(int *)(q + 0x44) = 0x40;
        *(int *)(q + 0x48) = 0x40;
        *(int *)(q + 0x4C) = 0x80;
        *(int *)(q + 0x50) = 0x8800;
        *(int *)(q + 0x54) = float_to_int((float)bias + (30720.0f + (16.0f * next->y)));
        *(int *)(q + 0x58) = 0;
        *(int *)(q + 0x5C) = 0;
        row += 16;
        i++;
    } while (i < 16);
}
