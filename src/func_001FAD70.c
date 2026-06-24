// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern char D_00282154[];
extern char D_0028202C[];
extern char D_00282024[];

void func_001FAD70(int arg0, int arg1, char arg2) {
    int off;
    if (D_00282154[arg0] != 0) {
        off = arg0 * 0x60;
        *(char *)(D_0028202C + off) = arg2;
        if (arg1 != 0) {
            if (arg0 != 0) {
                *(float *)(D_00282024 + off) = -(16383.0f / (float)arg1);
                return;
            }
            *(float *)(D_00282024 + off) = -(16383.0f / (float)arg1);
            return;
        }
        if (arg0 != 0) {
            *(int *)(D_00282024 + off) = 0xC67FFC00;
        } else {
            *(int *)(D_00282024 + off) = 0xC67FFC00;
        }
    }
}
