// NEARMISS func_001575E0  (vram 0x001575E0, 0xF4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 78.93% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation/scheduling permutation: target reuses $v0 for the three scratchpad float constants (load/store/reload), mwcc pre-loads into $a3/$a2/$a0 and reorders the stores. Straight-line logic fully recovered. Permuter territory, not a clean-store nop.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS: 78.93% (mwcc 2.3.3); 75.72% (991202). Logic fully recovered; the
// residual is a register-allocation/scheduling permutation: the target reuses
// $v0 for the three scratchpad float constants (load/store/reload), while mwcc
// pre-loads them into separate registers ($a3/$a2/$a0), shuffling the store
// order. Not a clean-store nop -- coloring/scheduling permuter territory.
//
// Sets up a camera/projection scratchpad block at 0x700038A0 and calls
// func_001B6F00(&scratch, pi/2) to build a matrix, then writes a second block of
// constants (20,14,15,1.0) and a set of float globals D_008105E0..EC derived from
// D_00810350/354/358, and calls func_001026A0(&D_008105D0, &actor[0xD0], &scratch).
// Returns 1.
extern int func_001026A0(void *, void *, void *);
extern int func_001B6F00(void *, float);
extern float D_00810350;
extern float D_00810354;
extern float D_00810358;
extern float D_008105D0;
extern float D_008105E0;
extern float D_008105E4;
extern float D_008105E8;
extern int D_008105EC;
extern float D_700038A0;

int func_001575E0(char *arg0) {
    *(float *)0x700038A0 = 10.0f;
    *(float *)0x700038A4 = 0.0f;
    *(float *)0x700038A8 = -5.0f;
    *(int *)0x700038AC = 0x3F800000;
    func_001B6F00(&D_700038A0, 1.5707964f);
    D_008105E0 = D_00810350;
    D_008105E4 = 11.0f + D_00810354;
    D_008105E8 = D_00810358;
    *(float *)0x700038A0 = 20.0f;
    *(float *)0x700038A4 = 14.0f;
    *(float *)0x700038A8 = 15.0f;
    D_008105EC = 0x3F800000;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(&D_008105D0, arg0 + 0xD0, &D_700038A0);
    return 1;
}
