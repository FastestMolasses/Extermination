// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
void func_0021C350(char *e) {
    float d = *(float *)(e + 0x224);

    if (d) {
        *(float *)(e + 0x220) = *(float *)(e + 0x220) - d;
        *(float *)(e + 0x224) = 0.0f;
        if (*(float *)(e + 0x220) <= 35.0f) {
            *(unsigned char *)(e + 0x235) &= 0xFE;
            *(unsigned char *)(e + 0x235) |= 1;
        }
        if (*(float *)(e + 0x220) <= 0.0f) {
            *(float *)(e + 0x220) = 0.0f;
            *(char *)e = 2;
        }
    }
}
