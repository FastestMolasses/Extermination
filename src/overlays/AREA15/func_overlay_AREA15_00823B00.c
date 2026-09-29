// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00823B40 (splat/link name 00823B00; overlay code is
// linked 0x40 below where it runs), 0x138 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Covers the splat pieces 00823B00, 00823B40 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: (runtime 0x823B40, D_008107FB 1) for +0xD 0x54: D_008107FB = 2 when
//  D_00810354 < 291, or when D_00810350 > 893, y > 329 and D_00810358 > 940,
//  or when the +0x28 countdown reaches 0 (then +5 = 0, +0x28 = 0). Then
//  func_001C64F0(1.0), func_001C68C0, the +0x4C method, func_001B17A0 and
//  0x823E40.
extern float D_00810350[];
extern unsigned char D_008107FB;
extern void func_001C64F0(unsigned char *self, float step);
extern int func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern void func_overlay_AREA15_00823E40(unsigned char *self);

void func_overlay_AREA15_00823B00(unsigned char *self) {
    if (self[0xD] == 0x54) {
        float y = D_00810350[1];
        if (y < 291.0f) {
            D_008107FB = 2;
        }
        if (D_00810350[0] > 893.0f && y > 329.0f && D_00810350[2] > 940.0f) {
            D_008107FB = 2;
        }
        (*(short *)(self + 0x28))--;
        if (*(short *)(self + 0x28) <= 0) {
            D_008107FB = 2;
            self[5] = 0;
            *(short *)(self + 0x28) = 0;
        }
    }
    func_001C64F0(self, 1.0f);
    func_001C68C0(self);
    func_001B17A0(self);
    (*(void (**)(unsigned char *))(self + 0x4C))(self);
    func_overlay_AREA15_00823E40(self);
}
