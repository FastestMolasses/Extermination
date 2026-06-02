extern float func_0011DE90(float);
extern float func_0011E2A8(float);

void func_0014CD30(unsigned char *a0, unsigned char *a1) {
    float *q = (float *)a1;
    float *p = (float *)a0;
    if (((signed char *)a1)[0x40] == 0) {
        q[9]  += q[10];
        q[11] += q[12];
        p[0x2D] += q[9];
        p[0x2C] += q[11] * func_0011E2A8(p[0x31]);
        p[0x2E] += q[11] * func_0011DE90(p[0x31]);
    }
}
