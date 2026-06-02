extern float func_0011DE90(float);
extern float func_0011E2A8(float);

void func_00136630(unsigned char *a0, unsigned char *a1) {
    float *q = (float *)a1;
    float *p = (float *)a0;
    if (((signed char *)a1)[0x59] == 0) {
        q[10] += -0.1f;
        q[11] += q[12];
        p[0x2D] += q[10];
        p[0x2C] += q[11] * func_0011E2A8(p[0x31]);
        p[0x2E] += q[11] * func_0011DE90(p[0x31]);
    }
}
