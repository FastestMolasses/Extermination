// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00823580 (splat/link name 00823540;
// overlay code is linked 0x40 below where it runs), 0x178 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Covers the splat pieces 00823540, 00823580 (the later piece is absorbed at link time,
// tools/overlay/fill_overlay.py).
// Role: talk turn (called as 0x823580 from 0x823700). Only when +0xB bit 2
// is set: turns the player block D_008102B0 (+0xC4) to face this actor or
// its opposite side (+0x2E = 0 or 1; subtype +3 0x16 flips +0x2E), moves
// the player through func_00182F90 to a point 6 units from this actor
// (func_0011E2A8 / func_0011DE90 of the new yaw), starts script 0x8272A0
// on the talk block (a1) and pumps it once; returns 1, else 0.
extern char D_008102B0[];
extern float D_700038A0[4];
extern char D_overlay_AREA04_008272A0[];
extern float func_001B1240(void *origin, float x, float z);
extern float func_001B1470(float a);
extern float func_0011DF78(float a);
extern float func_0011E2A8(float a);
extern float func_0011DE90(float a);
extern void func_00182F90(char *actor, void *target);
extern void func_001BA1A0(void *who, void *script);
extern void func_001BA1F0(void *self);

int overlay_AREA04_func_00823540(unsigned char *self, void *who) {
    char *pl = D_008102B0;
    if (self[0xB] & 4) {
        if (func_0011DF78(func_001B1470(func_001B1240(self + 0xB0, *(float *)(pl + 0xA0), *(float *)(pl + 0xA8)) - *(float *)(self + 0xC4))) <= 1.5707964f) {
            *(float *)(pl + 0xC4) = func_001B1470(3.1415927f + *(float *)(self + 0xC4));
            *(unsigned short *)(self + 0x2E) = 0;
        } else {
            *(float *)(pl + 0xC4) = func_001B1470(*(float *)(self + 0xC4));
            *(unsigned short *)(self + 0x2E) = 1;
        }
        if (self[3] == 0x16) {
            *(unsigned short *)(self + 0x2E) = 1 - *(unsigned short *)(self + 0x2E);
        }
        *(float *)0x700038A0 = *(float *)(self + 0xB0) - 6.0f * func_0011E2A8(*(float *)(pl + 0xC4));
        *(float *)0x700038A4 = *(float *)(pl + 0xA4);
        *(float *)0x700038A8 = *(float *)(self + 0xB8) - 6.0f * func_0011DE90(*(float *)(pl + 0xC4));
        *(float *)0x700038AC = 1.0f;
        func_00182F90(pl, D_700038A0);
        func_001BA1A0(who, D_overlay_AREA04_008272A0);
        func_001BA1F0(self);
        return 1;
    }
    return 0;
}
