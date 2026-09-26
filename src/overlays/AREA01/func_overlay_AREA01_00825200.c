// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00825240 (splat/link name 00825200; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: op09 callback of script 0x829FA0 (records 0x82A3A0/0x82A4E0); turns
//  +0xC4 to the parameter yaw, then plays the parameter clip.
extern float func_001B1470(float a);
extern void func_001C67E0(char *self, int clip, float blend, float frame);
extern float func_001B12B0(float goal, float cur, float rate);

int func_overlay_AREA01_00825200(char *self, unsigned char *st, char *prm) {
    float r;
    switch (st[4]) {
    case 0:
        if (func_001B1470(*(float *)(self + 0xC4) - *(float *)(prm + 0x24)) > 0.0f) {
            func_001C67E0(self, 5, 20.0f, 0.0f);
        } else {
            func_001C67E0(self, 6, 20.0f, 0.0f);
        }
        st[4] = 1;
        break;
    case 1:
        r = func_001B12B0(*(float *)(prm + 0x24), *(float *)(self + 0xC4), *(float *)(prm + 0xC));
        *(float *)(self + 0xC4) = r;
        if (r == *(float *)(prm + 0x24)) {
            /* zero staged through an int (MATCHING_GUIDE idiom-24) */
            int zi = 0;
            float z = (float)zi;
            func_001C67E0(self, *(short *)(prm + 0x1C), 20.0f, z);
            return 1;
        }
        break;
    }
    return 0;
}
