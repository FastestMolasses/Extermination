// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Draws the aiming-sight (AUTO_SIGHT) frame: two concentric rectangular outlines, each four
// 2-pixel bars, each a GS packet appended by func_00207F80(ctx, x0, y0,
// x1, y1, colour 0x801996FF). t = 0.5652174 + 0.43478262 * zoom (13/23 + 10/23 * zoom). Outer box half-size
// (122 t, 119 t) around (256, 222); inner box (24 t, 6 t). Corners are rounded
// with float_to_int; each bar's corners convert to GS primitive coordinates
// (x + 0x700, y / 2 + 0x790, times 16, again through float_to_int): top bar
// top..top+2, bottom bar bot..bot-2, left bar l..l+2, right bar r..r-2.
extern int float_to_int(float t);
extern void func_00207F80(int ctx, int x0, int y0, int x1, int y1, int rgba);

/* screen pixel -> GS primitive coordinate (12.4 fixed point, 2048-centred
   offset; y is a field line, so halved) */
#define GS_X(x) float_to_int(16.0f * (float)((x) + 0x700))
#define GS_Y(y) float_to_int(16.0f * (float)(((y) >> 1) + 0x790))

void func_001DC020(int ctx, float zoom) {
    float t;
    int l, top, r, bot;

    t = 0.5652174f + 0.43478262f * zoom;

    l = float_to_int(256.0f + -122.0f * t);
    top = float_to_int(222.0f + -119.0f * t);
    r = float_to_int(256.0f + 122.0f * t);
    bot = float_to_int(222.0f + 119.0f * t);
    func_00207F80(ctx, GS_X(l), GS_Y(top), GS_X(r), GS_Y(top + 2), 0x801996FF);
    func_00207F80(ctx, GS_X(l), GS_Y(bot), GS_X(r), GS_Y(bot - 2), 0x801996FF);
    func_00207F80(ctx, GS_X(l), GS_Y(top), GS_X(l + 2), GS_Y(bot), 0x801996FF);
    func_00207F80(ctx, GS_X(r), GS_Y(top), GS_X(r - 2), GS_Y(bot), 0x801996FF);

    l = float_to_int(256.0f + -24.0f * t);
    top = float_to_int(222.0f + -6.0f * t);
    r = float_to_int(256.0f + 24.0f * t);
    bot = float_to_int(222.0f + 6.0f * t);
    func_00207F80(ctx, GS_X(l), GS_Y(top), GS_X(r), GS_Y(top + 2), 0x801996FF);
    func_00207F80(ctx, GS_X(l), GS_Y(bot), GS_X(r), GS_Y(bot - 2), 0x801996FF);
    func_00207F80(ctx, GS_X(l), GS_Y(top), GS_X(l + 2), GS_Y(bot), 0x801996FF);
    func_00207F80(ctx, GS_X(r), GS_Y(top), GS_X(r - 2), GS_Y(bot), 0x801996FF);
}
