// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Draws the Gouraud glow fan (func_001D66A0) in mode 1: the three pointer
// arguments shift one register up and the float in f12 passes through
// unchanged to func_001D66A0's float parameter.
extern char *func_001D66A0(int mode, float *pos, int *a2, int *a3, float f);

void func_00208AB0(float *pos, int *a2, int *a3, float f) {
    func_001D66A0(1, pos, a2, a3, f);
}
