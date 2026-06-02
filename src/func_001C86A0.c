// Scratchpad scalar used to cache the reciprocal between the per-component
// stores (the EE scratchpad lives at 0x70000000).
#define SCRATCH (*(volatile float *)0x70003A3C)

// Componentwise scaled difference: dst = (a - b) / divisor, for a vec3.
void func_001C86A0(float *dst, const float *a, const float *b, float divisor) {
    SCRATCH = 1.0f / divisor;
    dst[0] = SCRATCH * (a[0] - b[0]);
    dst[1] = SCRATCH * (a[1] - b[1]);
    dst[2] = SCRATCH * (a[2] - b[2]);
}
