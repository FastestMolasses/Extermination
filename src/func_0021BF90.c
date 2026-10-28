// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Starts the hit reaction 0x3C for actor p from the hit record hit: sets bit
// 2 of p[0], the animation triple (bank 2, clip 0xD, frame 0), reaction mode
// p+0x1F0 = 0x3C with variant p+0x1F1 = 1, then maps the hit kind hit[3] to
// the reaction direction p[0xD] (1 -> 0, 4 -> 1, 7 -> 2, 6 -> 3, 5 -> 4,
// anything else 0); kinds 1, 7 and 6 also set p[0x25F] = 1. Finally
// refreshes the actor through func_0017C370(p) (a tail call).

extern void func_0017C370(char *p);

void func_0021BF90(unsigned char *p, unsigned char *hit) {
    unsigned char kind;

    p[0] |= 2;
    p[4] = 2;
    p[5] = 0xD;
    p[6] = 0;
    p[0x1F0] = 0x3C;
    p[0x1F1] = 1;
    kind = hit[3];
    if (kind == 1) {
        p[0xD] = 0;
        p[0x25F] = 1;
    } else if (kind == 4) {
        p[0xD] = 1;
    } else if (kind == 7) {
        p[0xD] = 2;
        p[0x25F] = 1;
    } else if (kind == 6) {
        p[0xD] = 3;
        p[0x25F] = 1;
    } else if (kind == 5) {
        p[0xD] = 4;
    } else {
        p[0xD] = 0;
    }
    func_0017C370((char *)p);
}
