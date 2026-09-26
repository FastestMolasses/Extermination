// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00824FE0 (splat/link name 00824FA0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: called from 0x824340; turns +0xC4 toward the player by at most 0.0436
//  rad.
// Covers the splat pieces 00824FA0, 00824FE0 (the later piece
// is absorbed at link time, tools/overlay/fill_overlay.py).
extern char *D_008106C0;
extern float func_001B1240(void *origin, float x, float z);
extern float func_001B12B0(float goal, float cur, float rate);

void func_overlay_AREA01_00824FA0(char *self) {
    char *pl = *(char **)(D_008106C0 + 0x118);
    float goal = func_001B1240(self + 0xB0, *(float *)(pl + 0xC0), *(float *)(pl + 0xC8));
    *(float *)(self + 0xC4) = func_001B12B0(goal, *(float *)(self + 0xC4), 0.0436332338f);
}
