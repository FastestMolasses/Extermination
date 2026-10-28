// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Follow-up of hit reaction 0x3C (see func_0021BF90) for actor p and hit
// record hit; does nothing unless p+0x1F0 is 0x3C. Hit kind 5 ends the
// reaction (animation bank 1 clip 0 frame 0, mode 0, halfword timer
// p+0x20E = 0x3C). Any other kind switches to reaction 0x3D (bank 2, clip
// 0xE, frame 0, variant 1) and maps hit[3] to the direction p[0xD] and the
// pending code p[0xF]: 4 -> (1, 4), 7 -> (2, 8), 6 -> (3, 9); kind 1 and
// every other kind set p[0xD] = 0 and leave p[0xF] alone.

void func_0021C040(unsigned char *p, unsigned char *hit) {
    unsigned char kind;

    if (p[0x1F0] == 0x3C) {
        if (hit[3] == 5) {
            p[4] = 1;
            p[5] = 0;
            p[6] = 0;
            p[0x1F0] = 0;
            *(short *)(p + 0x20E) = 0x3C;
        } else {
            p[4] = 2;
            p[5] = 0xE;
            p[6] = 0;
            p[0x1F0] = 0x3D;
            p[0x1F1] = 1;
            kind = hit[3];
            if (kind == 1) {
                p[0xD] = 0;
            } else if (kind == 4) {
                p[0xD] = 1;
                p[0xF] = 4;
            } else if (kind == 7) {
                p[0xD] = 2;
                p[0xF] = 8;
            } else if (kind == 6) {
                p[0xD] = 3;
                p[0xF] = 9;
            } else {
                p[0xD] = 0;
            }
        }
    }
}
