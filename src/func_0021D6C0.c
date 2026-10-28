// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Pending-damage reaction start for actor p. When neither pending damage
// amount (front p+0x224, back p+0x22C) is set and bit 2 of p[0xF] is clear
// it returns 0. Otherwise back damage runs func_0021C270(p) with variant
// p+0x1F1 = 1, front damage runs func_0021C350(p) with variant 0 (front
// wins when both are set); the facing p+0xC4 becomes
// wrap(atan2(-ctl.z, ctl.x) + pi/2) for the controller at p+0x20 (+0xC8,
// +0xC0), with the atan2 result staged at scratchpad 0x70003A20; p+0xC0 is
// cleared; in mode 0x33 the floats p+0x2E0/p+0x2E4 are copied to
// p+0x27C/p+0x278; then animation bank 2 clip 0xA frame 0, mode 0x46, and
// it returns 1.

extern float func_0011E620(float y, float x);
extern float func_001B1470(float a);
extern void func_0021C270(char *p);
extern void func_0021C350(char *p);

int func_0021D6C0(char *p) {
    char *ctl;

    if (*(float *)(p + 0x224) || *(float *)(p + 0x22C) || (*(unsigned char *)(p + 0xF) & 2)) {
        if (*(float *)(p + 0x22C)) {
            func_0021C270(p);
            p[0x1F1] = 1;
        }
        if (*(float *)(p + 0x224)) {
            func_0021C350(p);
            p[0x1F1] = 0;
        }
        ctl = *(char **)(p + 0x20);
        *(float *)0x70003A20 = func_0011E620(-*(float *)(ctl + 0xC8), *(float *)(ctl + 0xC0));
        *(float *)(p + 0xC4) = func_001B1470(1.5707964f + *(float *)0x70003A20);
        *(int *)(p + 0xC0) = 0;
        if (*(unsigned char *)(p + 0x1F0) == 0x33) {
            *(float *)(p + 0x27C) = *(float *)(p + 0x2E0);
            *(float *)(p + 0x278) = *(float *)(p + 0x2E4);
        }
        p[4] = 2;
        p[5] = 0xA;
        p[6] = 0;
        p[0x1F0] = 0x46;
        return 1;
    }
    return 0;
}
