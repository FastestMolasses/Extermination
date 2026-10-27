// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003600

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 99.27% NEARMISS).
// Fixes read from the original instructions:
//  - The scratch vec3's first word 0x70003600 is a relocated extern (idiom-32,
//    opted in with // SPAD:): its stores and reload go through D_70003600. With
//    the literal address mwcc speculated the 0x7000 upper half into the second
//    proximity-gate branch slot, where the original leaves it empty.
//    0x70003604..0C stay literals.
//  - The address arguments are plain &D_70003600 (no cast): a (void *) cast
//    swapped the order in which the a1/a2 arguments are set up.
//
// Proximity-test gate: zeroes the scratch vec3 at 0x70003600 (.x=0, .pad=0,
// .z=8.0, .w=1.0), builds a transform via func_001026A0(scratch, arg1+0xD0,
// scratch), then tests horizontal distance (dx=arg0[0xA0]-scratch.x,
// dz=arg0[0xA8]-scratch.z; func_0011E748 = sqrt of dx^2+dz^2) <= 8.0 AND
// vertical delta |arg0[0xA4]-arg1[0xB4]| (func_0011DF78=fabs) <= 4.0. On pass,
// rebuilds the scratch transform from arg2 and writes *arg3 =
// func_001B1470(PI + arg1[0xC4]) (a facing/yaw angle), returns 1; else 0.
extern int func_001026A0(void *, char *, void *);
extern float func_0011DF78(float);
extern float func_0011E748(float);
extern float func_001B1470(float);
extern float D_70003600;

int func_001AAA90(char *arg0, char *arg1, void *arg2, float *arg3) {
    float dx;
    float dz;

    D_70003600 = 0.0f;
    *(int *)0x70003604 = 0;
    *(float *)0x70003608 = 8.0f;
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(&D_70003600, arg1 + 0xD0, &D_70003600);
    dx = *(float *)(arg0 + 0xA0) - D_70003600;
    dz = *(float *)(arg0 + 0xA8) - *(float *)0x70003608;
    if (!(func_0011E748(dx * dx + dz * dz) <= 8.0f)) {
        return 0;
    }
    if (!(func_0011DF78(*(float *)(arg0 + 0xA4) - *(float *)(arg1 + 0xB4)) <= 4.0f)) {
        return 0;
    }
    D_70003600 = 0.0f;
    *(int *)0x70003604 = 0;
    *(float *)0x70003608 = 8.0f;
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(arg2, arg1 + 0xD0, &D_70003600);
    *arg3 = func_001B1470(3.1415927f + *(float *)(arg1 + 0xC4));
    return 1;
}
