extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern void func_001790B0(void *);

void func_00179150(unsigned char *a0) {
    float *f = (float *)a0;
    f[0x2C] += func_0011E2A8(f[0x31]) * (f[0xE] * func_0011DE90(f[0x27]));
    f[0x2E] += func_0011DE90(f[0x31]) * (f[0xE] * func_0011DE90(f[0x27]));
    func_001790B0(a0);
}
