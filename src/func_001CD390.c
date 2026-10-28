// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003610

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.91% NEARMISS).
// Fixes read from the original instructions:
// - The 0x70003610 field is a relocated scratchpad extern (idiom-32), not a
//   literal address: the store in both guard arms goes through D_70003610
//   (declared float). With the literal, mwcc 2.3.3 CSE'd the scratchpad high
//   half and hoisted it into the second float-compare branch's delay slot,
//   which the original leaves empty. The `// SPAD:` line opts this file's
//   expected object into the same %hi/%lo relocation (same address as the
//   byte-matched twin func_0012D580).
//
// Seeds the EE scratchpad transform block at 0x70003600.. from the vec3 at
// arg1 (arg1[0..2]) and builds an object orientation. First publishes a biased
// copy into the 0x70003610 field block: 0x70003610 = arg1[0],
// 0x70003614 = 5.0f + arg1[1], 0x7000361C = 1.0f, and 0x70003618 =
// (arg1[0]==0 && arg1[2]==0) ? 5.0f + arg1[2] : arg1[2]  (the +5.0 bias on Z
// only when both X and Z are zero). Then writes the raw vec into 0x70003600..
// (0x7000360C = 1.0f) and runs the matrix pipeline: two func_00102718 builds
// (D_70003620 from the 0x3610/0x3600 blocks, then D_70003630 from the
// 0x3620/0x3600 blocks), two func_00102760
// normalizes, func_001029C0(arg0) (identity load), then three func_001031E0
// applies that splat D_70003620/30/00 into the object matrix rows at arg0+0,
// arg0+0x10, arg0+0x20.
//

extern void func_00102718(void *a, void *b, void *c);
extern void func_00102760(void *a, void *b);
extern void func_001029C0(void *m);
extern void func_001031E0(void *a, void *b);
extern int D_70003600;
extern float D_70003610;
extern int D_70003620;
extern int D_70003630;

void func_001CD390(char *arg0, float *arg1) {
    if (arg1[0] == 0.0f && arg1[2] == 0.0f) {
        D_70003610 = arg1[0];
        *(float *)0x70003614 = 5.0f + arg1[1];
        *(float *)0x70003618 = 5.0f + arg1[2];
        *(int *)0x7000361C = 0x3F800000;
    } else {
        D_70003610 = arg1[0];
        *(float *)0x70003614 = 5.0f + arg1[1];
        *(float *)0x70003618 = arg1[2];
        *(int *)0x7000361C = 0x3F800000;
    }
    *(float *)0x70003600 = arg1[0];
    *(float *)0x70003604 = arg1[1];
    *(float *)0x70003608 = arg1[2];
    *(int *)0x7000360C = 0x3F800000;
    func_00102718(&D_70003620, &D_70003610, &D_70003600);
    func_00102718(&D_70003630, &D_70003620, &D_70003600);
    func_00102760(&D_70003620, &D_70003620);
    func_00102760(&D_70003630, &D_70003630);
    func_001029C0(arg0);
    func_001031E0(arg0, &D_70003620);
    func_001031E0(arg0 + 0x10, &D_70003630);
    func_001031E0(arg0 + 0x20, &D_70003600);
}
