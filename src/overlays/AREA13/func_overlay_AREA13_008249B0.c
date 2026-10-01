// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x008249F0 (splat/link name 008249B0; overlay code
//  is linked 0x40 below where it runs), 0x84 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Covers the splat pieces 008249B0, 008249F0 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called by 0x824390: walks up to 12 objects of the +0x18 chain and
//  sets +0x36 = 1 on each one whose +2 (without bits 5..7) is 4, +3 is 0xA
//  and +0xB4 >= 215. Returns 1.
int func_overlay_AREA13_008249B0(unsigned char *self) {
    unsigned char *o = *(unsigned char **)(self + 0x18);
    int i = 0;
    do {
        if ((unsigned char)(o[2] & ~0xE0) == 4 && o[3] == 0xA && !(*(float *)(o + 0xB4) < 215.0f)) {
            *(short *)(o + 0x36) = 1;
        }
        o = *(unsigned char **)(o + 0x18);
    } while (o != 0 && ++i < 12);
    return 1;
}
