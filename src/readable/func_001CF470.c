// NEARMISS func_001CF470  (vram 0x001CF470, 0x3F4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.49% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful.
// Matching wave 2026-10-28 (was 72.32%): the three position transforms are the original inline VU0 code (matrix rows into VU0 20..23, then per record a multiply-accumulate with w = 1 stored to the record clip slot), written as one-instruction asm statements with the record addresses taken into locals first; local declaration order fixes the saved-register assignment (axis, i, out, n, count, a, b, in).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Remaining differences: in case 0x02 and the second call of case 0x21 the original loads the -1.0f sign before the record address (so the 0x02 entry branch carries that load in its delay slot); the C emits the address first. Also one register name for record 0 clip address.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
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
    int axis;
    int i;
    int out;
    int n;
    int count;
    ClipRec *a;
    ClipRec *b;
    int in;
    float *src;
    float *dst;

    /* load the matrix rows into VU0 registers 20..23, then transform the
       three positions (implicit w = 1) into each record's clip slot */
    asm("lqc2 vf20, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf21, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf22, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf23, 0x30(%0)" : : "r"(m));
    dst = rec[0].clip;
    asm("lqc2 vf1, 0x0(%0)" : : "r"(rec[0].q[0]));
    asm("vmulax.xyzw ACC, vf20, vf1x");
    asm("vmadday.xyzw ACC, vf21, vf1y");
    asm("vmaddaz.xyzw ACC, vf22, vf1z");
    asm("vmaddw.xyzw vf2, vf23, vf0w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(dst));
    dst = rec[1].clip;
    src = rec[1].q[0];
    asm("lqc2 vf1, 0x0(%0)" : : "r"(src));
    asm("vmulax.xyzw ACC, vf20, vf1x");
    asm("vmadday.xyzw ACC, vf21, vf1y");
    asm("vmaddaz.xyzw ACC, vf22, vf1z");
    asm("vmaddw.xyzw vf2, vf23, vf0w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(dst));
    dst = rec[2].clip;
    src = rec[2].q[0];
    asm("lqc2 vf1, 0x0(%0)" : : "r"(src));
    asm("vmulax.xyzw ACC, vf20, vf1x");
    asm("vmadday.xyzw ACC, vf21, vf1y");
    asm("vmaddaz.xyzw ACC, vf22, vf1z");
    asm("vmaddw.xyzw vf2, vf23, vf0w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(dst));
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
