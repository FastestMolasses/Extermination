// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x008260A0 (splat/link name 00826060; overlay code
//  is linked 0x40 below where it runs), 0xD4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Covers the splat pieces 00826060, 008260A0 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called from 0x8297C0 with f = +0x22C / 900. func_001D1F80(1, 2, 1),
//  func_00207F80(1, 0x70F0, 0x84B0, 0x7B10, 0x8540, 0x30202020), then
//  func_00207F80(1, 0x7100, 0x84C0, w, 0x8540, 0x80202080) where w =
//  func_001281C0(16 * (float)(unsigned)(func_001281C0(16 + 160 * f) +
//  0x700)).
extern void func_001D1F80(int a0, int a1, int a2);
extern void func_00207F80(int a0, int a1, int a2, int a3, int t0, unsigned int t1);
extern int func_001281C0(float f);

void func_overlay_AREA21_00826060(float t) {
    unsigned int u;
    float f;
    func_001D1F80(1, 2, 1);
    func_00207F80(1, 0x70F0, 0x84B0, 0x7B10, 0x8540, 0x30202020);
    u = func_001281C0(16.0f + 160.0f * t) + 0x700;
    f = (float)u;
    func_00207F80(1, 0x7100, 0x84C0, func_001281C0(16.0f * f), 0x8540, 0x80202080);
}
