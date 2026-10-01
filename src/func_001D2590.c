// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Sets the render context's zoom from an angle (FRAME_RENDER_HEADS.md, "the
// zoom functions"): zoom = scale / tanf(angle / 2), handed to func_001D25F0,
// which stores it to the scratchpad word 0x70003B60 and to D_00275670 + 0x2468.
// func_0011E398 is the SDK tanf.
extern float func_0011E398(float x);
extern void func_001D25F0(float zoom);

void func_001D2590(float scale, float angle) {
    func_001D25F0(scale / func_0011E398(angle / 2.0f));
}
