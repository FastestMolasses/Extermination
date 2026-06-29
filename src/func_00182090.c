// CFLAGS: -O4,p -sdatathreshold 0
// Steer a float field (p+0xC4) toward target (p+0x218) by 3deg/step via func_001B12B0.
// Returns 1 if the result equals the target (reached), else 0 (disabled or still moving).
extern float func_001B12B0(float goal, float cur, float rate);

int func_00182090(int p) {
    float r;
    if (*(unsigned char *)(p + 0x23F) != 0) {
        r = func_001B12B0(*(float *)(p + 0x218), *(float *)(p + 0xC4), 0.06981316953897476f);
        *(float *)(p + 0xC4) = r;
        if (r == *(float *)(p + 0x218)) {
            return 1;
        }
    }
    return 0;
}
