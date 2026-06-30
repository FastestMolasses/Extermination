// NEARMISS func_001AAA90  (vram 0x001AAA90, 0x164 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.27% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Single-instruction residual: both mwcc builds speculatively hoist the store-block address `lui at,0x7000` into the second `bc1t` delay slot where the target keeps a `nop`. Pure mwcc delay-slot/branch-fill scheduling; source reshaping only lowers score. Permuter/compiler-version wall.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 99.27% (mwcc 2.3.3; 991202 = 88.60%). Body/structure fully recovered
// and byte-identical except ONE residual instruction: the second `bc1t` (the
// `func_0011DF78(...) <= 4.0f` proximity gate) delay slot. The target leaves a
// `nop` in that slot and emits the store block's `lui at,0x7000` after; both
// available mwcc builds speculatively hoist that address-`lui` into the bc1t
// delay slot (branch target shifts +4). Source-level reshaping (positive guard,
// store-block reorder) only lowers the score; this is the mwcc delay-slot /
// branch-fill scheduler, permuter/compiler-version territory.
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

    *(float *)0x70003600 = 0.0f;
    *(int *)0x70003604 = 0;
    *(float *)0x70003608 = 8.0f;
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(&D_70003600, arg1 + 0xD0, &D_70003600);
    dx = *(float *)(arg0 + 0xA0) - *(float *)0x70003600;
    dz = *(float *)(arg0 + 0xA8) - *(float *)0x70003608;
    if (!(func_0011E748(dx * dx + dz * dz) <= 8.0f)) {
        return 0;
    }
    if (!(func_0011DF78(*(float *)(arg0 + 0xA4) - *(float *)(arg1 + 0xB4)) <= 4.0f)) {
        return 0;
    }
    *(float *)0x70003600 = 0.0f;
    *(int *)0x70003604 = 0;
    *(float *)0x70003608 = 8.0f;
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(arg2, arg1 + 0xD0, &D_70003600);
    *arg3 = func_001B1470(3.1415927f + *(float *)(arg1 + 0xC4));
    return 1;
}
