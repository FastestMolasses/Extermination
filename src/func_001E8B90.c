// NEARMISS func_001E8B90  (vram 0x001E8B90, 0x2EC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.01% via mwcc 2.3.3 (-O4,p -sdatathreshold 4). The body follows the original instructions;
// the residual diff is code generation only:
// Body corrected 2026-09-25 against the original instructions (record stride 0xA060, edge/corner neighbour weights; was 71.53% with the wrong body). Wave 2026-10-27 (86.49% -> 99.01%): compiler is 2.3.3 (it re-forms the +0x9060 cell offset per access exactly like the original when the cell is indexed as float element 0x2418), the three weights are computed before the point is read, x' is computed before z', the neighbour loops test <= 1, local declaration order from a permuter run. Remaining differences: integer temp register colouring only (the original never uses a0/a1 for the area byte, col and the neighbour indices; this C uses them, shifting a2..t2 down by two).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Water-ripple stamp. Does nothing while the area byte D_00810700 is 0x15 or
// 0x10. Otherwise it visits the 4 grid records at D_00275C20 (record stride
// 0xA060). A record with word +0x54 != 0 takes the point arg0+0/4/8 when
// x0 < x <= x0 + w (+0x0, +0x30), z0 < z <= z0 + d (+0x8, +0x34) and
// y < 11.0 + y0 (+0x4). The point is mapped to a 32x32 cell: x' = 32(x - x0)/w,
// z' = 32(z - z0)/d, row = float_to_int(z'), col = float_to_int(x'), each
// clamped to 0..31; x' and z' replace x and z for the later records. In the
// record's float grid at +0x9060 (column stride 4, row stride 0x80, every
// neighbour index clamped to 0..31) it adds -7*speed to the cell, -5*speed to
// its four edge neighbours and -3*speed to its four corner neighbours.
extern int float_to_int(float);
extern unsigned char D_00810700[];
extern unsigned char *D_00275C20;

void func_001E8B90(unsigned char *arg0, float speed) {
    int i;
    int row;
    unsigned char *p;
    int idx;
    int col;
    float px;
    float py;
    float pz;
    float wCenter;
    float wEdge;
    float wCorner;
    float dx0;
    float dz0;
    float dx1;
    unsigned char *colp;
    float dz1;
    int colIdx;
    int rowIdx;
    int rowOff;
    int colOff;

    if (D_00810700[0] == 0x15 || D_00810700[0] == 0x10) {
        return;
    }
    wCenter = -7.0f * speed;
    wEdge = -5.0f * speed;
    wCorner = -3.0f * speed;
    px = *(float *)(arg0 + 0);
    py = *(float *)(arg0 + 4);
    pz = *(float *)(arg0 + 8);

    i = 0;
    idx = 0;
    do {
        p = D_00275C20 + idx;
        if (*(int *)(p + 0x54) != 0) {
            dx0 = *(float *)(p + 0);
            if (px > dx0) {
                dx1 = *(float *)(p + 0x30);
                if (px <= (dx0 + dx1)) {
                    dz0 = *(float *)(p + 8);
                    if (pz > dz0) {
                        dz1 = *(float *)(p + 0x34);
                        if ((pz <= (dz0 + dz1)) && (py < (11.0f + *(float *)(p + 4)))) {
                            px = (32.0f * (px - dx0)) / dx1;
                            pz = (32.0f * (pz - dz0)) / dz1;
                            row = float_to_int(pz);
                            col = float_to_int(px);

                            if (row < 0) {
                                row = 0;
                            } else if (row >= 0x20) {
                                row = 0x1F;
                            }

                            if (col < 0) {
                                col = 0;
                            } else if (col >= 0x20) {
                                col = 0x1F;
                            }

                            for (colOff = -1; colOff <= 1; colOff++) {
                                colIdx = col + colOff;
                                if (colIdx < 0) {
                                    colIdx = 0;
                                } else if (colIdx >= 0x20) {
                                    colIdx = 0x1F;
                                }
                                colp = p + (colIdx * 4);

                                for (rowOff = -1; rowOff <= 1; rowOff++) {
                                    rowIdx = row + rowOff;
                                    if (rowIdx < 0) {
                                        rowIdx = 0;
                                    } else if (rowIdx >= 0x20) {
                                        rowIdx = 0x1F;
                                    }

                                    if (rowOff == 0 && colOff == 0) {
                                        ((float *)(colp + (rowIdx << 7)))[0x2418] += wCenter;
                                    } else if (rowOff == 0 || colOff == 0) {
                                        ((float *)(colp + (rowIdx << 7)))[0x2418] += wEdge;
                                    } else {
                                        ((float *)(colp + (rowIdx << 7)))[0x2418] += wCorner;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        idx = idx + 0xA060;
        i += 1;
    } while (i < 4);
}
