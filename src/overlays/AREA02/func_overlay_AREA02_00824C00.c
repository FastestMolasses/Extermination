// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA02 overlay, runtime 0x00824C40 (splat/link name 00824C00; overlay
// code is linked 0x40 below where it runs), 0x88 bytes. Byte-identical
// (tools/overlay/overlay_match.py check AREA02; lane A02C).
// Covers the splat pieces 00824C00, 00824C40 (the later piece is absorbed at
// link time, tools/overlay/fill_overlay.py).
// Role: sprite at pos: grey level 255 - (rand & 15) in all three channels,
//  func_001CD520(0, 2, pos, giftag, 14, 14, 4, colour).
extern int func_00122BB8(void);
extern int func_001CD520(int bucket, int mode, void *world, unsigned long long giftag,
                         float w, float h, float zbias, unsigned int rgba);

void func_overlay_AREA02_00824C00(void *pos) {
    unsigned int k;
    unsigned int v;
    unsigned int c;

    k = 0xFF - (func_00122BB8() & 0xF);
    v = 0x80 * k;
    c = (v >> 7) << 16;
    c |= (v >> 7) << 8;
    c |= v >> 7;
    func_001CD520(0, 2, pos, 0x20045B0599421EF0ULL, 14.0f, 14.0f, 4.0f, c);
}
