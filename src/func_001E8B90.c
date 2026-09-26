// NEARMISS func_001E8B90  (vram 0x001E8B90, 0x2EC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.49% via mwcc 2.4 (-O4,p -sdatathreshold 8). The body follows the original instructions;
// the residual diff is code generation only:
// Body corrected 2026-09-25 against the original instructions (record stride 0xA060, edge/corner neighbour weights; was 71.53% with the wrong body). Residual: register coloring (s0..s3 and FP temps) and the +0x9060 cell offset, which the target re-forms per access with a lui/addu pair while mwcc folds it into the pointer.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc24
// CFLAGS: -O4,p -sdatathreshold 8

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
    float px, py, pz;
    float wCenter, wEdge, wCorner;
    float dx0, dz0, dx1, dz1;
    int row, col;
    int colIdx;
    int rowIdx;
    int colOff;
    int rowOff;
    unsigned char *p;
    unsigned char *colp;
    unsigned char *cell;
    int i;
    int idx;

    if (D_00810700[0] == 0x15 || D_00810700[0] == 0x10) {
        return;
    }
    wCenter = -7.0f * speed;
    px = *(float *)(arg0 + 0);
    py = *(float *)(arg0 + 4);
    pz = *(float *)(arg0 + 8);
    wEdge = -5.0f * speed;
    wCorner = -3.0f * speed;

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
                            pz = (32.0f * (pz - dz0)) / dz1;
                            px = (32.0f * (px - dx0)) / dx1;
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

                            for (colOff = -1; colOff < 2; colOff++) {
                                colIdx = col + colOff;
                                if (colIdx < 0) {
                                    colIdx = 0;
                                } else if (colIdx >= 0x20) {
                                    colIdx = 0x1F;
                                }
                                colp = p + (colIdx * 4);

                                for (rowOff = -1; rowOff < 2; rowOff++) {
                                    rowIdx = row + rowOff;
                                    if (rowIdx < 0) {
                                        rowIdx = 0;
                                    } else if (rowIdx >= 0x20) {
                                        rowIdx = 0x1F;
                                    }

                                    if (rowOff == 0 && colOff == 0) {
                                        *(float *)(colp + (rowIdx << 7) + 0x9060) += wCenter;
                                    } else if (rowOff == 0 || colOff == 0) {
                                        *(float *)(colp + (rowIdx << 7) + 0x9060) += wEdge;
                                    } else {
                                        *(float *)(colp + (rowIdx << 7) + 0x9060) += wCorner;
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
