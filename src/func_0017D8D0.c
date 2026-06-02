// Per-frame motion update with a countdown at 0x28. On the final tick (counter
// reaches 0) snap the position (0xB0..0xB8) to the target stored at 0x2F4/0x258/
// 0x2F8 and report 1; otherwise integrate the velocity at 0x2E0..0x2E8 and
// report 0. The counter is decremented every call.
int func_0017D8D0(unsigned char *obj) {
    short counter = *(short *)(obj + 0x28);
    *(short *)(obj + 0x28) = counter - 1;
    if (counter == 0) {
        *(float *)(obj + 0xB4) = *(float *)(obj + 0x258);
        *(float *)(obj + 0xB0) = *(float *)(obj + 0x2F4);
        *(float *)(obj + 0xB8) = *(float *)(obj + 0x2F8);
        return 1;
    }
    *(float *)(obj + 0xB4) += *(float *)(obj + 0x2E4);
    *(float *)(obj + 0xB0) += *(float *)(obj + 0x2E0);
    *(float *)(obj + 0xB8) += *(float *)(obj + 0x2E8);
    return 0;
}
