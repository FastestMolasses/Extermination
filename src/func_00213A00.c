// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Refills a 9-entry ring buffer from a circular source list, then runs two display passes.
extern int float_to_int(float);
extern void func_001FCF60(unsigned char, int, int);
extern void func_00207D00(int, int);
extern void func_00207E40(int, int, int, int, int, int, long long);
extern void func_0020CDA0(void);
extern unsigned short D_00810E78;

int func_00213A00(char *arg0, int arg1) {
    int var_a0;
    int var_s0;
    int var_s1;
    int var_s3;
    unsigned char temp_v0;
    unsigned char temp_v1;
    int var_a1;
    char *var_v0;

    *(char *)(arg0 + 0x1A) = 0;
    var_s0 = 0;
    var_s1 = 0;
    if ((*(unsigned char *)(arg0 + 0x18) != 0) && !(arg1 & 0x400)) {
        if (D_00810E78 & 0x1000) {
            temp_v0 = *(unsigned char *)(arg0 + 0x17);
            if (temp_v0 == 0) {
                var_s1 = 1;
                *(char *)(arg0 + 0x1A) = 1;
            } else {
                *(unsigned char *)(arg0 + 0x17) = (unsigned char)(temp_v0 - 1);
            }
            func_0020CDA0();
        } else if (D_00810E78 & 0x4000) {
            temp_v1 = *(unsigned char *)(arg0 + 0x17);
            if ((int)temp_v1 >= 7) {
                *(unsigned char *)(arg0 + 0x17) = 7U;
                var_s1 = 1;
                *(char *)(arg0 + 0x1A) = 2;
            } else {
                *(unsigned char *)(arg0 + 0x17) = (unsigned char)(temp_v1 + 1);
            }
            func_0020CDA0();
        }
    }
    var_a1 = *(unsigned char *)(arg0 + 0x19);
    var_a0 = 0;
    do {
        var_v0 = arg0 + var_a1;
        var_a1 += 1;
        *(unsigned char *)(arg0 + var_a0 + 0x90) = (unsigned char)*(unsigned char *)(var_v0 + 0x50);
        if ((int)var_a1 >= (int)*(unsigned char *)(arg0 + 0x18)) {
            var_a1 = 0;
        }
        var_a0 += 1;
        var_v0 = arg0;
        var_v0 = var_v0 + var_a1;
    } while (var_a0 < 9);
    var_s3 = 0;
    do {
        func_001FCF60(*(unsigned char *)(arg0 + var_s3 + 0x90), 0x64, var_s0 + 0x47);
        var_s3 += 1;
        var_s0 += 0xC;
    } while (var_s3 < 8);
    func_00207D00(1, 0);
    func_00207E40(1, 0x7000, float_to_int(16.0f * (float)(((int)((*(unsigned char *)(arg0 + 0x17) * 0x18) + 0x88) >> 1) + 0x790)), 0x100, 0x40, (int)0x80808080, 0x20044585A1322100LL);
    func_00207E40(1, 0x8000, float_to_int(16.0f * (float)(((int)((*(unsigned char *)(arg0 + 0x17) * 0x18) + 0x88) >> 1) + 0x790)), 0x100, 0x40, (int)0x80808080, 0x20044705A1322140LL);
    return var_s1;
}
