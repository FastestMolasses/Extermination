// COMPILER: eegcc
// CFLAGS: -O2
extern int func_001157F0(int a0, int a1, int a2, int a3);

int func_0011A960(int a0, unsigned long a1, unsigned int a2) {
    int v1 = -1;
    if (a1 <= 0xFFFF) {
        if (a2 <= 0x1FFFFF) {
            if (a0 < 2) {
                func_001157F0(0x4F, (int)a1, a2, a0);
                v1 = 0;
            }
        }
    }
    return v1;
}
