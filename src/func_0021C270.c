// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001FBD50(char *, int, int, float);
extern void func_0021D4E0(char *);

extern unsigned char D_008106F1;
extern unsigned char D_00810707;

void func_0021C270(char *arg0) {
    float zero = 0.0f;
    float dt = *(float *)(arg0 + 0x22C);
    if (dt == zero) return;
    *(float *)(arg0 + 0x228) = *(float *)(arg0 + 0x228) + dt;
    *(float *)(arg0 + 0x22C) = zero;
    if (*(float *)(arg0 + 0x228) >= 100.0f) {
        *(float *)(arg0 + 0x228) = 100.0f;
        if (*(float *)(arg0 + 0x220) > 60.0f) *(float *)(arg0 + 0x220) = 60.0f;
        if (*(unsigned char *)(arg0 + 0x234) == 0) {
            *(unsigned char *)(arg0 + 0x234) = 1;
            D_00810707 = 1;
            D_008106F1 = 1;
        }
    }
    func_0021D4E0(arg0);
    func_001FBD50(arg0, 0x149, 0, 300.0f);
}
