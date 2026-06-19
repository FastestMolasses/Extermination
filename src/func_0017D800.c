// CFLAGS: -O4,p -sdatathreshold 0
extern short float_to_int(float f);

void func_0017D800(unsigned char *e) {
    float g;
    *(int *)(e + 0x2E4) = 0x3F19999A;
    g = *(float *)(e + 0x258) / *(float *)(e + 0x2E4);
    *(volatile float *)0x70003A20 = g;
    if (g < 1.0f) {
        *(volatile float *)0x70003A20 = 1.0f;
    }
    *(float *)(e + 0x258) += *(float *)(e + 0xB4);
    *(short *)(e + 0x28) = float_to_int(*(volatile float *)0x70003A20);
    *(float *)(e + 0x2F4) = *(float *)(e + 0x2E0);
    *(float *)(e + 0x2F8) = *(float *)(e + 0x2E8);
    *(float *)(e + 0x2E0) = (*(float *)(e + 0x2F4) - *(float *)(e + 0xB0)) / *(volatile float *)0x70003A20;
    *(float *)(e + 0x2E8) = (*(float *)(e + 0x2F8) - *(float *)(e + 0xB8)) / *(volatile float *)0x70003A20;
    e[0x25F] = 1;
}
