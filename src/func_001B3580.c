// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Writes a vec3+1.0 into the scratchpad block at 0x70003600 from the caller's
// float[3] (arg1), runs func_001028B8 in place on that block (with actor
// arg0+0xB0), then queries func_001B1240 with the block's x/z, and finally
// calls func_001B1470 with: that result plus 1.0471976 * (2*(b/255)-1), where
// b = (func_00122BB8() >> 7) & 0xFF. The 2.0/255.0 literals are GPR-loaded
// constants inside the expression; func_001B1470 takes the single computed
// float in $f12.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202 (89.19%):
// 2.3.3 reproduces CW's FP register allocation / constant-load ordering for the
// final div/mul/sub chain that 991202 permutes. objdiff 100.0% vs expected.
extern void func_001028B8(float *, float *, char *);
extern int func_00122BB8(void);
extern float func_001B1240(char *, float, float);
extern void func_001B1470(float);
extern float D_70003600;

void func_001B3580(char *arg0, float *arg1) {
    float temp_f20;

    *(float *)0x70003600 = arg1[0];
    *(float *)0x70003604 = arg1[1];
    *(float *)0x70003608 = arg1[2];
    *(int *)0x7000360C = 0x3F800000;
    func_001028B8(&D_70003600, &D_70003600, arg0 + 0xB0);
    temp_f20 = func_001B1240(arg0 + 0xB0, *(float *)0x70003600, *(float *)0x70003608);
    func_001B1470(temp_f20 + 1.0471976f * (2.0f * ((float)((func_00122BB8() >> 7) & 0xFF) / 255.0f) - 1.0f));
}
