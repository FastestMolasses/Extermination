extern float func_0011DE90(float);
extern float func_0011E2A8(float);

void func_00150900(unsigned char *a0, unsigned char *a1) {
    float *q = (float *)a1;
    float *p = (float *)a0;
    if (((signed char *)a1)[0x63] == 0) {
        q[14] += q[15];
        q[16] += q[18];
        p[0x2D] += q[14];
        p[0x2C] += q[16] * func_0011E2A8(q[10]);
        p[0x2E] += q[16] * func_0011DE90(q[10]);
    }
}
