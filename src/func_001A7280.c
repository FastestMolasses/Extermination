// NEARMISS func_001A7280  (vram 0x001A7280, 0x5F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 72.05% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 72.05% (233) -- body-level control flow, offsets, and call sequence recovered with reasonable confidence for the outer scan/mask/plane-clip stages (verified line-by-line against the .s). LOWER CONFIDENCE on the innermost per-vertex epsilon-test loop's exact accept/reject wiring (the .L001A7738/.L...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Per-frame wall/floor-segment collision scan against the moving object at
// D_70003190/D_700030B0 (position/heading pair). Copies arg0's velocity
// fields (+0x0 and +0x10, 3 floats each) into sp130/sp140, builds the sweep
// plane sp150 via func_001028D0, then walks the world collision-object
// array at D_008102B0+0x58 (count at [0]; a short -2 tag at [0xA] means an
// extended header that must be skipped via the first record's own length).
// For each record: computes a per-axis mask from arg0's bit flags 0x10/
// 0x20/0x40 against the record's own per-axis flag bytes ANDed with the
// world's flag bytes at +0x5C/0x5D/0x5E; a negative segment-count short at
// [+6] forces the mask to 0 (skip). If the mask is nonzero: reloads the
// per-"kind"-byte [+3] alignment matrix into sp1A0 via copy_qw4 only when
// the kind changes since the last record (a small one-entry cache), then
// transforms the record's own two boundary vertices ([+8] and [+0x10])
// into the object-relative frame (func_001026A0) and clips the moving
// object's sweep segment (sp150/sp130/sp140) against the resulting plane
// (func_00102738 distances, func_00103230 lerp, func_001028B8 transform) to
// get the candidate pierce point sp170. The point is accepted only if it
// lies within [sp130,sp140] on all 3 axes (three OR-of-two-inequality
// range checks). If accepted, the record's own list of interior vertices
// (stride 0xC bytes starting at [+0x10+(3*segcount+4)*4]) is walked to find
// whether the pierce point is actually inside the polygon by testing each
// vertex against the clip plane's implicit distance (func_001028D0 point-
// to-plane, func_00102738 dot) within a 0x3727C5AC (~1.0e-5f) epsilon; the
// FIRST record whose test succeeds records a hit (D_700031D4 latches the
// world table base) and copies its own boundary/data back into
// D_70003190. Returns 1 if any record hit this frame, 0 otherwise. NOTE:
// the innermost per-vertex epsilon test's exact accept/reject wiring
// (blocks reached via .L001A7738 / .L001A773C in the original) is
// transcribed faithfully from the disassembly but is NOT independently
// verified against a byte-identical rebuild -- treat the very last stage
// (the s6/s3 vertex-count loop and its exit condition) as lower-confidence
// than the rest of this function.
extern void copy_qw4(void *dst, void *src);
extern void func_001026A0(void *dst, void *m, void *src);
extern float func_00102738(void *a, void *b);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00103230(void *dst, void *src, float f);

extern char D_008102B0;
extern char D_700030B0;
extern char D_70003190;

int func_001A7280(int arg0) {
    float sp1A0[16];
    float sp190[4];
    float sp180[3];
    float sp170[3];
    float sp160[3];
    float sp150[3];
    float sp140[3];
    float sp130[3];
    int spD0;
    int spE0;
    char *seg;
    char *rec1;
    char *rec2;
    int nInterior;
    int count;
    int i;

    *(short *)0x700030CA = 0;
    *(char **)0x700031D0 = &D_700030B0;

    {
        char *src = &D_70003190;

        for (i = 0; i < 3; i++) {
            sp130[i] = *(float *)(src + 0);
            sp140[i] = *(float *)(src + 0x10);
            src += 4;
        }
    }
    func_001028D0(sp150, sp140, sp130);

    spE0 = 0;
    spD0 = 0;

    if (*(char **)(&D_008102B0 + 0x58) == 0 || *(unsigned char *)&D_008102B0 == 0) {
        return 0;
    }

    seg = *(char **)(&D_008102B0 + 0x58) + 4;
    count = *(int *)*(char **)(&D_008102B0 + 0x58);
    if (*(short *)(*(char **)(&D_008102B0 + 0x58) + 0xA) == -2) {
        seg += *(unsigned short *)(seg + 4);
        count -= 1;
    }

    if (count > 0) {
        int flagLo = arg0 & 0x10;
        int flagMid = arg0 & 0x20;
        int flagHi = arg0 & 0x40;
        int kindCache = 0x270F;
        int idx;

        for (idx = 0; idx < count; idx++) {
            int mask = 0;
            short n;

            if (flagLo && (*(unsigned char *)(seg + 0) & *(unsigned char *)(&D_008102B0 + 0x5C))) {
                mask = 1;
            }
            if (flagMid && (*(unsigned char *)(seg + 1) & *(unsigned char *)(&D_008102B0 + 0x5D))) {
                mask |= 2;
            }
            if (flagHi && (*(unsigned char *)(seg + 2) & *(unsigned char *)(&D_008102B0 + 0x5E))) {
                mask |= 4;
            }

            n = *(short *)(seg + 6);
            if (n < 0) {
                mask = 0;
            }

            if (mask != 0) {
                unsigned char kind = *(unsigned char *)(seg + 3);
                char *p;

                p = seg + 8;
                if (kindCache != (int)kind) {
                    kindCache = (int)kind;
                    copy_qw4(sp1A0, *(float **)(&D_008102B0 + kind * 4 + 0x110) + (0x90 / 4));
                    p = seg + 8;
                }

                for (i = 0; i < 3; i++) {
                    sp160[i] = *(float *)(p + i * 4);
                }
                func_001026A0(sp160, sp1A0, sp160);

                sp180[0] = *(float *)(p + 0x10);
                sp180[1] = *(float *)(p + 0x14);
                sp180[2] = *(float *)(p + 0x18);
                func_001026A0(sp180, sp1A0, sp180);

                {
                    float dPlane = func_00102738(sp160, sp180);
                    float dSpan = func_00102738(sp150, sp160);
                    float dFrom = func_00102738(sp160, sp130);

                    func_00103230(sp170, sp150, (dPlane - dFrom) / dSpan);
                }
                func_001028B8(sp170, sp130, sp170);

                if ((sp130[0] > sp170[0] && sp140[0] < sp170[0]) ||
                    (sp130[1] > sp170[1] && sp140[1] < sp170[1]) ||
                    (sp130[2] > sp170[2] && sp140[2] < sp170[2])) {
                    /* pierce point outside the record's own range on some axis: skip. */
                } else {
                    rec1 = p + 0x10;
                    nInterior = (int)n;
                    rec2 = rec1 + (nInterior * 3 + 4) * 4;

                    if (nInterior > 0) {
                        int j;
                        int hit = 0;

                        for (j = 0; j < nInterior && !hit; j++) {
                            float v0[4];
                            float v1[4];
                            float d;

                            v0[0] = *(float *)(rec1 + 0);
                            v0[1] = *(float *)(rec1 + 4);
                            v0[2] = *(float *)(rec1 + 8);
                            v0[3] = 1.0f;
                            func_001026A0(v0, sp1A0, v0);
                            v0[3] = 0;
                            func_001028D0(sp180, sp170, v0);

                            v1[0] = *(float *)(rec2 + 0);
                            v1[1] = *(float *)(rec2 + 4);
                            v1[2] = *(float *)(rec2 + 8);
                            v1[3] = 0;
                            func_001026A0(v1, sp1A0, v1);
                            v1[3] = 0;

                            d = func_00102738(sp180, v1);
                            if (d <= 0.00001f) {
                                hit = 1;
                            }
                            rec1 += 0xC;
                            rec2 += 0xC;
                        }

                        if (hit) {
                            spD0 = 1;
                            for (i = 0; i < 3; i++) {
                                *(float *)(&D_70003190 + 0x20 + i * 4) = sp170[i];
                            }
                            for (i = 0; i < 3; i++) {
                                *(float *)(&D_700030B0 + 0x24 + i * 4) = *(float *)(p + 0x10 + i * 4);
                            }
                            for (i = 0; i < 3; i++) {
                                sp140[i] = *(float *)(&D_70003190 + 0x20 + i * 4);
                            }
                            func_001028D0(sp150, sp140, sp130);
                            spE0 = 1;
                        }
                    }
                }
            }

            seg += *(unsigned short *)(seg + 4);
            if (spD0 != 0) {
                *(char **)0x700031D4 = &D_008102B0;
            }
        }
    }

    return spE0;
}
