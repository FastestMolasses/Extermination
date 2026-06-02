void func_00179880(unsigned char *a0, float *a1) {
    a1[0] += -0.04f;
    if (a1[0] < -4.0f) {
        a1[0] = -4.0f;
    }
    *(float *)(a0 + 0xB4) += a1[0];
    a0[0x25F] = 2;
}
