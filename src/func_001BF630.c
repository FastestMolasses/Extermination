// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern unsigned char D_70003B8D;
extern float func_0011E748(float x);

int func_001BF630(char *a, char *b, char *zone) {
    float dx, dz;
    if (D_70003B8D != 0) {
        return 0;
    }
    dx = *(float *)(a + 0xA0) - *(float *)(b + 0xB0);
    dz = *(float *)(a + 0xA8) - *(float *)(b + 0xB8);
    if (func_0011E748(dx * dx + dz * dz) > **(float **)(zone + 0x18)) return 0;
    return 1;
}
