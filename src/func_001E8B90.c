// NEARMISS func_001E8B90  (vram 0x001E8B90, 0x2EC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 71.53% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring/scheduling residual in the FP compare chain (198==198 instructions, structurally matched) plus one address-decomposition difference: target splits the +0x9060 write offset into two chained +0x1068/+0x7FF8 immediate adds instead of a single lui+addu, suggesting a slightly differe...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS (72.1% mwcc / 71.5% mwcc233): water-ripple stamp. Skips while
// D_00810700 (area/substate byte) is 0x15 or 0x10. Otherwise scans the 4
// D_00275C20 grid records (stride 0x2061) for the one whose XZ box (+0x0/+0x30
// width, +0x8/+0x34 depth) contains the actor position (arg0+0/4/8), gated by
// a Y-band (+0x4 + 11.0 > py). Maps the local XZ offset into a 0..31 cell
// (float_to_int, clamped [0,0x1F]) and stamps a 3x3 neighborhood of ripple
// decay values into the record's 32x32 float grid at +0x9060 + row*4 + col*0x80
// (each axis independently clamped to the grid edge): center cell -=7*speed,
// same-column vertical neighbors -=3*speed, all other (diagonal/horizontal)
// neighbors -=5*speed.
//
// Body/control-flow fully recovered and instruction-count-exact (198==198
// vs target); residual is register-coloring/scheduling in the FP compare
// chain plus one address-decomposition difference (target splits the +0x9060
// cell offset as two chained +0x1068/+0x7FF8 immediates instead of a single
// lui+addu -- likely a different intermediate pointer expression) that
// resisted further cracking within the attempt budget.
extern int float_to_int(float);
extern unsigned char D_00810700;
extern unsigned char *D_00275C20;

void func_001E8B90(unsigned char *arg0, float speed) {
    float px, py, pz;
    float wCenter, wVert, wOther;
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

    if (D_00810700 == 0x15 || D_00810700 == 0x10) {
        return;
    }
    wCenter = -7.0f * speed;
    px = *(float *)(arg0 + 0);
    py = *(float *)(arg0 + 4);
    pz = *(float *)(arg0 + 8);
    wOther = -5.0f * speed;
    wVert = -3.0f * speed;

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

                            colIdx = col - 1;
                            for (colOff = -1; colOff < 2; colOff++) {
                                if (colIdx < 0) {
                                    colIdx = 0;
                                } else if (colIdx >= 0x20) {
                                    colIdx = 0x1F;
                                }
                                colp = p + (colIdx * 4);

                                rowIdx = row - 1;
                                for (rowOff = -1; rowOff < 2; rowOff++) {
                                    if (rowIdx < 0) {
                                        rowIdx = 0;
                                    } else if (rowIdx >= 0x20) {
                                        rowIdx = 0x1F;
                                    }

                                    cell = colp + (rowIdx << 7);
                                    if (rowOff == 0 && colOff == 0) {
                                        *(float *)(cell + 0x9060) = *(float *)(cell + 0x9060) + wCenter;
                                    } else if (rowOff != 0 && colOff == 0) {
                                        *(float *)(cell + 0x9060) = *(float *)(cell + 0x9060) + wVert;
                                    } else {
                                        *(float *)(cell + 0x9060) = *(float *)(cell + 0x9060) + wOther;
                                    }

                                    rowIdx = row + (rowOff + 1);
                                }
                                colIdx = col + (colOff + 1);
                            }
                        }
                    }
                }
            }
        }
        idx = idx + 0x2061;
        i += 1;
    } while (i < 4);
}
