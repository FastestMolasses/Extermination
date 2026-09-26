// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00824F70 (splat/link name 00824F30; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: called from 0x824340; true when the player (D_008106C0 +0x118) is
//  within 8.0 on the XZ plane.
// Covers the splat pieces 00824F30, 00824F70 (the later piece
// is absorbed at link time, tools/overlay/fill_overlay.py).
extern char *D_008106C0;
extern float func_0011E748(float x);

int func_overlay_AREA01_00824F30(char *self) {
    char *pl = *(char **)(D_008106C0 + 0x118);
    float dx;
    float dz;
    float d;
    dx = *(float *)(pl + 0xC0);
    dx = *(float *)(self + 0xB0) - dx;
    dz = *(float *)(self + 0xB8) - *(float *)(pl + 0xC8);
    d = func_0011E748(dx * dx + dz * dz);
    return (d > 8.0f) ? 0 : 1;
}
