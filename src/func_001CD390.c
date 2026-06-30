// NEARMISS func_001CD390  (vram 0x001CD390, 0x188 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// branch-delay-slot scheduling permutation: the dual float-equality guard `if (arg1[0]==0.0f && arg1[2]==0.0f)` emits two CW `bc1f` branches to the same else-block, and the CW target leaves the SECOND bc1f's delay slot as an explicit `nop`; mwcc 2.3.3 fills it by hoisting the common `lui at,0x7000`...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Seeds the EE scratchpad transform block at 0x70003600.. from the vec3 at
// arg1 (arg1[0..2]) and builds an object orientation. First publishes a biased
// copy into the 0x70003610 field block: 0x70003610 = arg1[0],
// 0x70003614 = 5.0f + arg1[1], 0x7000361C = 1.0f, and 0x70003618 =
// (arg1[0]==0 && arg1[2]==0) ? 5.0f + arg1[2] : arg1[2]  (the +5.0 bias on Z
// only when both X and Z are zero). Then writes the raw vec into 0x70003600..
// (0x7000360C = 1.0f) and runs the matrix pipeline: two func_00102718 builds
// (D_70003620 and D_70003630 from the 0x3610/0x3600 blocks), two func_00102760
// normalizes, func_001029C0(arg0) (identity load), then three func_001031E0
// applies that splat D_70003620/30/00 into the object matrix rows at arg0+0,
// arg0+0x10, arg0+0x20.
//
// WALL: branch-delay-slot scheduling permutation. The dual float guard
// `if (arg1[0]==0.0f && arg1[2]==0.0f)` emits two `bc1f` to the shared else
// block; the CW (2.3.1) target leaves the SECOND bc1f's delay slot as an
// explicit `nop`, while mwcc 2.3.3 fills it by hoisting the common
// `lui at,0x7000` scratchpad-store address, shifting one branch offset. Same
// class as the func_0012D580 NEARMISS; not the clean-store nop, not
// source-fixable - parked.
extern void func_00102718(void *a, void *b, void *c);
extern void func_00102760(void *a, void *b);
extern void func_001029C0(void *m);
extern void func_001031E0(void *a, void *b);
extern int D_70003600;
extern int D_70003610;
extern int D_70003620;
extern int D_70003630;

void func_001CD390(char *arg0, float *arg1) {
    if (arg1[0] == 0.0f && arg1[2] == 0.0f) {
        *(float *)0x70003610 = arg1[0];
        *(float *)0x70003614 = 5.0f + arg1[1];
        *(float *)0x70003618 = 5.0f + arg1[2];
        *(int *)0x7000361C = 0x3F800000;
    } else {
        *(float *)0x70003610 = arg1[0];
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
