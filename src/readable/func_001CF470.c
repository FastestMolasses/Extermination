// NEARMISS func_001CF470  (vram 0x001CF470, 0x3F4 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 72.32% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written VU0 section: the original transforms the three positions with VU0 macro
// multiply-accumulate code, which C can only express as FPU arithmetic; the clipping loop and its
// outcode dispatch follow the original.
//
// The function links from the asm body in src/func_001CF470.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Shadow-decal triangle clipper (port SHADOW_DECAL.md: em_shadow_decal_
// 001CF470). rec is a scratch array of 0x50-byte vertex records (four
// attribute quadwords, then the clip position at +0x40); records 0..2 hold
// the triangle. Their positions are transformed by the 4x4 matrix m into
// clip space (in the original with VU0 macro code: a multiply-accumulate of
// the matrix rows by x, y, z and w = 1). The polygon is then clipped against
// z, y and x in turn (Sutherland-Hodgman, ping-ponging between records 0..
// and 16..): for each edge a -> b the outcode func_001CF870(a, b, axis)
// decides what is emitted (block_copy keeps a vertex, func_001CF970 emits
// the crossing with the +w (1.0) or -w (-1.0) plane). Returns the vertex
// count of the clipped polygon (in the records starting at 16 after the odd
// final pass), or 0 when nothing is left.
typedef struct ClipRec {
    float q[4][4];      /* attributes; q[0] is the position */
    float clip[4];      /* 0x40 */
} ClipRec;

extern unsigned char func_001CF870(ClipRec *a, ClipRec *b, int axis);
extern void func_001CF970(ClipRec *out, ClipRec *a, ClipRec *b, int axis, float sign);
extern void block_copy(void *dst, void *src, int n);

int func_001CF470(ClipRec *rec, float *m) {
    int count;
    int n;
    int i;
    int axis;
    int in;
    int out;
    int k;
    ClipRec *a;
    ClipRec *b;

    for (k = 0; k < 3; k++) {
        for (i = 0; i < 4; i++) {
            rec[k].clip[i] = m[i] * rec[k].q[0][0] + m[4 + i] * rec[k].q[0][1]
                           + m[8 + i] * rec[k].q[0][2] + m[12 + i];
        }
    }
    count = 3;
    in = 16;
    out = 0;
    for (axis = 2; axis >= 0; axis--) {
        n = count;
        count = 0;
        in = 16 - in;
        out = 16 - out;
        for (i = 0; i < n; i++) {
            a = &rec[in + i % n];
            b = &rec[in + (i + 1) % n];
            switch (func_001CF870(a, b, axis)) {
            case 0x00:
                block_copy(&rec[out + count], a, 0x50);
                count += 1;
                break;
            case 0x01:
                func_001CF970(&rec[out + count], a, b, axis, 1.0f);
                count += 1;
                break;
            case 0x02:
                func_001CF970(&rec[out + count], a, b, axis, -1.0f);
                count += 1;
                break;
            case 0x10:
                block_copy(&rec[out + count], a, 0x50);
                func_001CF970(&rec[out + count + 1], a, b, axis, 1.0f);
                count += 2;
                break;
            case 0x20:
                block_copy(&rec[out + count], a, 0x50);
                func_001CF970(&rec[out + count + 1], a, b, axis, -1.0f);
                count += 2;
                break;
            case 0x12:
                func_001CF970(&rec[out + count], a, b, axis, -1.0f);
                func_001CF970(&rec[out + count + 1], a, b, axis, 1.0f);
                count += 2;
                break;
            case 0x21:
                func_001CF970(&rec[out + count], a, b, axis, 1.0f);
                func_001CF970(&rec[out + count + 1], a, b, axis, -1.0f);
                count += 2;
                break;
            case 0x11:
            case 0x22:
                break;
            }
        }
        if (count == 0) {
            return 0;
        }
    }
    return count;
}
