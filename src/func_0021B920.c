extern int *D_00275670;

void func_0021B920(float f12, float f13) {
    int *v1;
    float f1;
    f1 = 255.0f / (f13 - f12);
    v1 = D_00275670;
    *(int *)((char *)v1 + 0xA0) = 0x437F0000;
    v1 = D_00275670;
    *(int *)((char *)v1 + 0xA4) = 0x45000000;
    v1 = D_00275670;
    *(float *)((char *)v1 + 0xA8) = f13 * f1;
    v1 = D_00275670;
    *(float *)((char *)v1 + 0xAC) = -f1;
}
