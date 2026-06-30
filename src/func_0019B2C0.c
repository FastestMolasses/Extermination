// NEARMISS func_0019B2C0  (vram 0x0019B2C0, 0x200 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.64% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// idiom-13 const-store delay-slot NOP x2: target leaves a NOP in the beqz delay slot before a global-address `lui at,0x7000` (the else-store at 0x700031D0 and the flags&0x80000000 guard); mwcc 2.3.3 and 991202 both speculatively fill the slot with the lui. Body otherwise byte-identical. Genuine bac...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: builds the global scratch transform at 0x70003190 / D_700031A0 from the two input vectors
// (a0, a1). Seeds 0x70003190.x = a0.x, .y = a1.y, .z = a0.z; copies a1.xyz into D_700031A0[4..6] and a
// stack temp; zeroes scratch fields 0x700031AC/0x7000319C/0x700031D4; chains the matrix helpers
// func_001028D0/func_00102760/func_00103230(.,.,0.01f)/func_001028B8 to produce a clamped/blended
// transform; sets the short flag at 0x7000324E = -1. Then a mode select: if (flags & 2) clears 0x70003254
// and probes func_0019FE50() (mode 2 on success); if (flags & 4) probes func_0019CB60() (mode 4). Recomputes
// the transform, and if a mode was chosen copies the saved a1 vector back into D_70003190[4..6] and stores
// the delta D_70003190[8..10]-[4..6] into [12..14]; if flags<0 (bit 0x80000000) nudges a0.x/a0.z by the
// global offsets 0x700031C0/0x700031C8. Otherwise clears 0x700031D0. Stores the mode at 0x700031D8 and
// returns it.
//
// WALL: idiom-13 const-store delay-slot NOP, x2. At `if(mode!=0){...}else{*(int*)0x700031D0=0;}` and the
// `flags&0x80000000` guard, the target leaves a NOP in the conditional-branch delay slot whose successor
// first instr is a GLOBAL address `lui at,0x7000`; mwcc 2.3.3 (and 991202) speculatively fill that slot
// with the lui. Per docs/fanout/MATCHING_GUIDE.md this is matchable ONLY when the slot candidate is a
// memory load — here it is a global lui, so it is a genuine backend wall. Body is otherwise byte-identical.
extern void func_001028D0(void *a, void *b, void *c);
extern void func_00102760(void *a, void *b);
extern void func_00103230(void *a, void *b, float v);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_0019FE50(void);
extern int func_0019CB60(void);

extern float D_70003190[];
extern float D_700031A0[];

int func_0019B2C0(float *a0, float *a1, int flags) {
    float mat[4];
    float tmp[4];
    int i;
    int mode;

    *(float *)0x70003190 = a0[0];
    *(float *)0x70003194 = a1[1];
    *(float *)0x70003198 = a0[2];

    mode = 0;
    for (i = 0; i < 3; i++) {
        float t = a1[i];
        D_700031A0[4 + i] = t;
        tmp[i] = t;
    }
    *(int *)0x700031AC = 0;
    *(int *)0x7000319C = 0;
    *(int *)0x700031D4 = 0;
    func_001028D0(mat, D_700031A0, D_70003190);
    func_00102760(mat, mat);
    func_00103230(mat, mat, 0.01f);
    func_001028B8(D_700031A0, D_700031A0, mat);
    *(short *)0x7000324E = -1;

    if (flags & 2) {
        *(int *)0x70003254 = 0;
        if (func_0019FE50() == 0) {
            mode = 2;
        }
    }
    if (flags & 4) {
        if (func_0019CB60() == 0) {
            mode = 4;
        }
    }
    func_001028D0(D_700031A0, D_700031A0, mat);
    if (mode != 0) {
        for (i = 0; i < 3; i++) {
            D_70003190[4 + i] = tmp[i];
            D_70003190[12 + i] = D_70003190[8 + i] - D_70003190[4 + i];
        }
        if (flags & 0x80000000) {
            a0[0] = a0[0] + *(float *)0x700031C0;
            a0[2] = a0[2] + *(float *)0x700031C8;
        }
    } else {
        *(int *)0x700031D0 = 0;
    }
    *(int *)0x700031D8 = mode;
    return mode;
}
