// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA08 overlay, runtime 0x00823630 (splat/link name 008235F0; overlay code
//  is linked 0x40 below where it runs), 0x178 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA08; lane OVLC).
// Covers the splat pieces 008235F0, 00823630 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called from 0x823870; the AREA04 0x823580 C with the script 0x825880.
extern char D_008102B0[];
extern float D_700038A0[4];
extern char D_overlay_AREA08_00825880[];
extern float func_001B1240(void *origin, float x, float z);
extern float func_001B1470(float a);
extern float func_0011DF78(float a);
extern float func_0011E2A8(float a);
extern float func_0011DE90(float a);
extern void func_00182F90(char *actor, void *target);
extern void func_001BA1A0(void *who, void *script);
extern void func_001BA1F0(void *self);

int func_overlay_AREA08_008235F0(unsigned char *self, void *who) {
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
        func_001BA1A0(who, D_overlay_AREA08_00825880);
        func_001BA1F0(self);
        return 1;
    }
    return 0;
}
