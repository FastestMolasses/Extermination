// Per-frame motion update with a countdown at 0x28. On the final tick snap the
// position (0xB0/0xB4/0xB8) to the target at 0x2F4/0x258/0x2F8 and report 1;
// otherwise integrate velocity (0x2E0/0x2E4/0x2E8) plus a decaying lateral push
// (0x26C, which itself bleeds off by 0x270 each frame) and report 0.
int func_0017DE20(unsigned char *obj) {
    short counter = *(short *)(obj + 0x28);
    *(short *)(obj + 0x28) = counter - 1;
    if (counter == 0) {
        *(float *)(obj + 0xB0) = *(float *)(obj + 0x2F4);
        *(float *)(obj + 0xB8) = *(float *)(obj + 0x2F8);
        *(float *)(obj + 0xB4) = *(float *)(obj + 0x258);
        return 1;
    }
    *(float *)(obj + 0xB0) += *(float *)(obj + 0x2E0);
    *(float *)(obj + 0xB8) += *(float *)(obj + 0x2E8);
    *(float *)(obj + 0xB4) += *(float *)(obj + 0x2E4);
    *(float *)(obj + 0xB4) += *(float *)(obj + 0x26C);
    *(float *)(obj + 0x26C) -= *(float *)(obj + 0x270);
    return 0;
}
