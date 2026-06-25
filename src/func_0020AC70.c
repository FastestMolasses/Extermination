// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Per-frame particle spawn/advance. arg1 is a vec2 base position (xy floats);
// arg2 is a reset flag. Advances a 16-entry ring slot D_00275C90 (kept gp-rel
// via the small-data threshold; the int counter is small data, the D_008213xx
// arrays are NOT, so they stay %hi/%lo). When arg2==0 it runs func_001B62C0 on
// the slot; otherwise it zeroes the 4 slot words (each via its own D_00821300/
// 04/08/0C symbol so mwcc reuses one shifted index reg, matching CW). Then it
// loops 16 particle records (stride 0x10): builds a vec4 pos {x,y,28+16*z,28}
// plus an int color triple from float_to_int(16*(1+z)), submits via
// func_00208AB0, and decays z by 0.8. The leading unused int param matches the
// a0 the body never reads (ptr arrives in a1, flag in a2). pos[]/col[] arrays
// and decl order fix the stack layout and s0/s1 (ptr/counter) coloring; the
// `col[0]=col[1]=col[2]=` order and `p[2] *= 0.8f` compound-assign pin the last
// two FP operand orderings. Built with mwcc 2.3.3 (mwcps2-2.3.3-000906);
// objdiff 100% byte-identical (991202 build only reaches 92.3%).
extern int float_to_int(float);
extern void func_001B62C0(void *);
extern void func_00207D00(int, int);
extern void func_00208AB0(float *, int *, void *, float);
extern char D_00273580[];
extern int D_00275C90;
extern char D_00821300[];
extern int D_00821304[];
extern int D_00821308[];
extern int D_0082130C[];

void func_0020AC70(int unused, float *arg1, int arg2) {
    float pos[4];
    int col[3];
    int idx;
    int i;
    float *p;

    arg1[0] = 1792.0f + arg1[0];
    arg1[1] = 1824.0f + arg1[1];
    D_00275C90 = (D_00275C90 + 1) & 0xF;
    if (arg2 == 0) {
        func_001B62C0(D_00821300 + (D_00275C90 * 0x10));
    } else {
        idx = D_00275C90 * 4;
        *(int *)(D_00821300 + idx * 4) = 0;
        D_00821304[idx] = 0;
        D_00821308[idx] = 0;
        D_0082130C[idx] = 0;
    }
    func_00207D00(1, 1);
    p = (float *)&D_00821300;
    i = 0;
    do {
        pos[0] = arg1[0] + 0.8f * (44.0f * p[0]);
        pos[1] = arg1[1] + 0.5f * (44.0f * p[1]);
        pos[2] = 28.0f + 16.0f * p[2];
        pos[3] = 28.0f;
        col[0] = col[1] = col[2] = float_to_int(16.0f * (1.0f + p[2]));
        func_00208AB0(pos, col, D_00273580, p[3]);
        p[2] *= 0.8f;
        i += 1;
        p += 4;
    } while (i < 0x10);
}
