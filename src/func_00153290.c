// NEARMISS func_00153290  (vram 0x00153290, 0x2B0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.31% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation/scheduling permutation only. Body and control flow structurally identical to target: two particle-spawn loops (6-iter then 4-iter) each computing a color word and small rotation matrix per bone entry. All residual diffs are same-operation register-name swaps (s1/s2 pointer-rol...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Twin particle-spawn loop. First loop (up to 6 iterations, breaking early if
// func_001AFA90(2) returns NULL) allocates a class-2 actor per entry of
// D_00246730[5][4], sets model byte=8 and behavior=func_00153540, copies its
// position (elems 1..3) into +0xB0..0xB8 with w=1.0, blends in the bone
// matrix at D_00275B40[idx]+0x90 (idx = elem 0) via copy_qw4 + func_001026A0,
// builds a small rotation matrix in D_700036A0 (identity, scale by a fixed
// -0.235619456f angle, then by func_001B1470(arg0->0xC4 + elem 4)), packs a
// randomized alpha/color word into D_700038A0 (0.75 + 0.35 * rand-byte/255),
// and finalizes the actor's +0xC0 matrix via func_001026A0. If all 6 succeed,
// a second loop (4 iterations over D_002467B0[5][4]) builds a similar
// per-entry color+matrix pair in D_700038A0/D_700038B0 and spawns two more
// objects via func_0014D1E0 and func_001EFD90.
//
// NEARMISS 93.31% with mwcc 2.3.3 (mwcps2-2.3.3-000906). Keys applied:
// (1) func_00102948/func_00102C58 real signatures take pointer args (not
// int) -- fixing these unlocked the correct sp90/sp50 stack layout and frame
// size. (2) func_00102B08/func_00102BB0's 3rd arg is $f12 (float), not an
// int -- the 0xBE71463A constant must be passed as its exact float bit-cast
// (-0.235619455575943f) to get the target's early mtc1-then-addr-compute
// call-arg order. (3) func_001026A0 takes exactly 3 args; an apparent 4th
// arg in the m2c decode was a mis-attributed scratch-register load
// (0.75f's bit pattern staged in $a3 for a later mtc1), not a real
// parameter. (4) The two loops are for(;i<N;i++) not do/while, matching the
// target's loop-condition materialization point. (5) The float triplet
// copied into D_700038A0/D_700038B0 must be plain float loads/stores
// (lwc1/swc1), not int-reinterpreted casts, to match the target exactly.
// Residual: pure register-coloring/scheduling permutation (s1/s2 role swap
// resolved by declaration-order idiom; remaining diffs are same-op
// register-name swaps) -- parked for the permuter pass.

extern void copy_qw4(void *, void *);
extern void func_001026A0(void *, void *, void *);
extern void func_001029C0(void *);
extern void func_00102B08(void *, void *, float);
extern void func_00102BB0(void *, void *, float);
extern int func_00122BB8(void);
extern void func_0014D1E0(void *, void *, int);
extern char *func_001AFA90(int);
extern float func_001B1470(float);
extern void func_001EFD90(int, void *, void *);

extern float D_00246730[5][4];
extern float D_002467B0[5][4];
extern char **D_00275B40;
extern int D_700036A0[4];
extern int D_700038A0[4];
extern int D_700038B0[4];
extern void func_00153540(void);

void func_00153290(char *arg0) {
    int i;
    char *obj;
    float *p;
    int idx;

    p = D_00246730[0];
    for (i = 0; i < 6; i++) {
        obj = func_001AFA90(2);
        if (obj == 0) {
            break;
        }
        obj[3] = 8;
        *(void **)(obj + 0x10) = (void *)func_00153540;
        *(float *)(obj + 0xB0) = p[1];
        *(float *)(obj + 0xB4) = p[2];
        *(float *)(obj + 0xB8) = p[3];
        *(int *)(obj + 0xBC) = 0x3F800000;

        idx = *(int *)p;
        copy_qw4(obj + 0xB0, (char *)D_00275B40[idx] + 0x90);
        func_001026A0(obj + 0xB0, D_700036A0, obj + 0xB0);

        func_001029C0(D_700036A0);
        func_00102B08(D_700036A0, D_700036A0, -0.235619455575943f);
        func_00102BB0(D_700036A0, D_700036A0, func_001B1470(*(float *)(arg0 + 0xC4) + p[4]));

        *(volatile int *)0x700038A0 = 0;
        *(volatile int *)0x700038A4 = 0;
        *(volatile float *)0x700038A8 = 0.75f + (0.35f * ((float)((func_00122BB8() >> 0xF) & 0xFF) / 255.0f));
        *(volatile int *)0x700038AC = 0x3F800000;

        func_001026A0(obj + 0xC0, D_700036A0, D_700038A0);

        p += 5;
    }

    if (i >= 6) {
        p = D_002467B0[0];
        for (i = 0; i < 4; i++) {
            *(volatile float *)0x700038A0 = p[1];
            *(volatile float *)0x700038A4 = p[2];
            *(volatile float *)0x700038A8 = p[3];
            *(volatile int *)0x700038AC = 0x3F800000;

            idx = *(int *)p;
            copy_qw4(D_700038A0, (char *)D_00275B40[idx] + 0x90);
            func_001026A0(D_700038A0, D_700036A0, D_700038A0);

            *(volatile int *)0x700038B8 = 0;
            *(volatile float *)0x700038B4 = 0.0f;
            *(volatile int *)0x700038B0 = 0;
            *(volatile int *)0x700038BC = 0x3F800000;
            *(volatile int *)0x700038B0 = 0xBFC6D3F2;
            *(volatile float *)0x700038B4 = func_001B1470(*(float *)(arg0 + 0xC4) + p[4]);

            func_0014D1E0(D_700038A0, D_700038B0, 1);
            func_001EFD90(3, D_700038A0, D_700038B0);

            p += 5;
        }
    }
}
