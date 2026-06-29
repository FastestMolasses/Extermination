// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Array-of-(short*) double index: returns D_00248AB0[a0][a1] (lh in jr delay slot).
extern short *D_00248AB0[];

short func_0017B460(int a0, int a1) {
    return D_00248AB0[a0][a1];
}
