// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Box test between a and b: when |a+0xB0 - b+0xB0| <= 50, |a+0xB8 - b+0xB8|
// <= 46 and |a+0xB4 - b+0xB4| <= 40 (func_0011DF78), b+0x36 = 1.
extern float func_0011DF78(float x);

void func_001AA640(char *a, char *b) {
    if (func_0011DF78(*(float *)(a + 0xB0) - *(float *)(b + 0xB0)) <= 50.0f &&
        func_0011DF78(*(float *)(a + 0xB8) - *(float *)(b + 0xB8)) <= 46.0f &&
        func_0011DF78(*(float *)(a + 0xB4) - *(float *)(b + 0xB4)) <= 40.0f) {
        *(short *)(b + 0x36) = 1;
    }
}
