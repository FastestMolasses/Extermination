// NEARMISS func_001F88C0  (vram 0x001F88C0, 0x46C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.45% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-allocation + FP-register coloring permutation. Body and control flow are byte-identical to target; residuals are saved-reg coloring (scratch base s2 vs build-table ptr s0 are swapped; seed/counter reuse s1 vs s3), FPU register coloring (f1 vs f8/f9 in the angle/phase math), and 2 commuta...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 93.45% (mwcc 2.3.3; 991202 = 86.78%). Body/structure fully recovered
// and byte-correct; the sole residuals are register-allocation coloring and
// FP-register coloring permutations (saved-reg reuse s0<->s2 for the scratch
// base vs. the build-table pointer, and s1<->s3 reuse for the seed/loop
// counters; a few commutative add.s operand orderings and f1-vs-f8/f9 FPU
// coloring). No idiom fixes remain -> permuter territory.
//
// Per-entity animation/particle driver. arg0 is an entity; state byte at +4
// selects: 2/3 -> despawn via func_001AFC10(self,state); 0 -> one-time init
// then fall through to 1 -> per-frame update; other -> no-op.
//   s1 = self+0x24 (sub-object: anim descriptor; +3 kind, +4 finished flag,
//        +0xC count, +0x110[] bone/clip table). s2 = self+0x1F0 (per-behavior
//        scratch: +0 PRNG seed, +4 variant index, +8 ring write cursor,
//        +0xC slot count, +0x10[] slot id array).
// Init (state 0): pick a slot-id table by kind (1/2/3/9 -> D_0025DA40/60/C0/A0,
//   else the procedural 2.. path that walks count entries advancing by
//   (rand%2)+1), reserve 0x38 bone slots via func_001F87E0, mark variant
//   (kind==9 -> 1 else 0), reset 0x150 particles' field +0x1C to -1.0f
//   (0xBF800000), clear cursors, reseed PRNG from func_00122BB8().
// Update (state 1): if anim finished (+4==3) flag state 3 and return. Every 4th
//   global tick (D_70003B68 %4==0) spawn one particle per slot: build a
//   rotation in scratch matrix D_700036A0 (identity, rotate by a random angle
//   pi*(180*r/2^31 - 90)/180, transform bone +0x110[id]+0x90), allocate the
//   ring particle func_001F8880(cursor), seed its pos from scratchpad
//   0x700036D0/D4/D8, advance the ring cursor (wrap at 0x150). Then advance all
//   0x150 live particles: LCG the seed (seed*0x25+0xB), stash a normalized
//   phase at 0x70003A20, and for each particle with phase +0x1C>=0 build its
//   transform (quat_to_mat3 + func_001D04B0 into instance buffer
//   D_0025D920 + variant*0x90), step phase by 0.05 and retire (-1.0f) past 1.5.
extern int func_001026D0();
extern int func_001029C0();
extern int func_00102BB0(char *, char *, float);
extern int func_00122BB8(void);
extern void func_001AFC10(char *, unsigned char);
extern int func_001C9E40();
extern int func_001D04B0(char *, int, char *, float, float);
extern int func_001F87E0(char *, unsigned char);
extern char *func_001F8880(int);
extern int quat_to_mat3();
extern int D_0025D920;
extern int D_0025DA40;
extern int D_0025DA60;
extern int D_0025DAA0;
extern int D_0025DAC0;
extern char D_700036A0;

void func_001F88C0(char *arg0) {
    char *s1;
    char *s2;
    int *s0;
    int n;
    int i;
    char *e;
    unsigned char st;

    st = *(unsigned char *)(arg0 + 4);
    s1 = *(char **)(arg0 + 0x24);
    s2 = arg0 + 0x1F0;
    s0 = 0;
    switch (st) {
    case 0:
        if (*(unsigned char *)(s1 + 3) != 9) {
            *(int *)(s2 + 4) = 0;
        } else {
            *(int *)(s2 + 4) = 1;
        }
        switch (*(unsigned char *)(s1 + 3)) {
        case 1:
            s0 = &D_0025DA40;
            break;
        case 3:
            s0 = &D_0025DAC0;
            break;
        case 2:
            s0 = &D_0025DA60;
            break;
        case 9:
            s0 = &D_0025DAA0;
            break;
        }
        if (func_001F87E0(arg0, 0x38) != 0) {
            *(int *)(s2 + 0xC) = 0;
            if (s0 != 0) {
                while ((n = *s0) != -1) {
                    s0 += 1;
                    *(int *)(s2 + *(int *)(s2 + 0xC) * 4 + 0x10) = n;
                    *(int *)(s2 + 0xC) = *(int *)(s2 + 0xC) + 1;
                }
            } else {
                i = 2;
                while (i < (int)*(unsigned char *)(s1 + 0xC)) {
                    *(int *)(s2 + *(int *)(s2 + 0xC) * 4 + 0x10) = i;
                    *(int *)(s2 + 0xC) = *(int *)(s2 + 0xC) + 1;
                    if (*(int *)(s2 + 0xC) < 0xC) {
                        i += (func_00122BB8() % 2) + 1;
                    } else {
                        break;
                    }
                }
            }
            {
                int j = 0;
                do {
                    *(int *)(func_001F8880(j) + 0x1C) = 0xBF800000;
                    j += 1;
                } while (j < 0x150);
            }
            *(int *)(s2 + 8) = 0;
            *(int *)(s2 + 4) = 0;
            *(int *)(s2 + 0) = func_00122BB8();
        }
    case 1:
        if (*(unsigned char *)(s1 + 4) == 3) {
            *(unsigned char *)(arg0 + 4) = 3;
            return;
        }
        {
            int s3 = 0;
            if ((*(int *)0x70003B68 % 4) == 0) {
                char *p = s2;
                while (s3 < *(int *)(s2 + 0xC)) {
                    char *ent;
                    float angle;
                    func_001029C0(&D_700036A0);
                    angle = (3.1415927f * (180.0f * ((float)func_00122BB8() / 2147483648.0f) - 90.0f)) / 180.0f;
                    func_00102BB0(&D_700036A0, &D_700036A0, angle);
                    func_001026D0(&D_700036A0, *(int *)(s1 + *(int *)(p + 0x10) * 4 + 0x110) + 0x90, &D_700036A0);
                    ent = func_001F8880(*(int *)(s2 + 8));
                    func_001C9E40(ent, &D_700036A0);
                    *(float *)(ent + 0x10) = *(float *)0x700036D0;
                    *(float *)(ent + 0x14) = *(float *)0x700036D4;
                    *(float *)(ent + 0x18) = *(float *)0x700036D8;
                    *(int *)(ent + 0x1C) = 0;
                    *(int *)(s2 + 8) = *(int *)(s2 + 8) + 1;
                    if (*(int *)(s2 + 8) >= 0x150) {
                        *(int *)(s2 + 8) = 0;
                    }
                    p += 4;
                    s3 += 1;
                }
            }
        }
        {
            int seed = *(int *)(s2 + 0);
            i = 0;
            do {
                char *ent = func_001F8880(i);
                *(float *)0x70003A20 = (float)((seed >> 0x10) & 0xFFFF) / 65535.0f + 0.0001f;
                seed = seed * 0x25 + 0xB;
                if (*(float *)(ent + 0x1C) >= 0.0f) {
                    quat_to_mat3(&D_700036A0, ent, ent + 0x10);
                    func_001D04B0(&D_700036A0, 1, (char *)&D_0025D920 + *(int *)(s2 + 4) * 0x90, *(float *)(ent + 0x1C), *(float *)0x70003A20);
                    *(float *)(ent + 0x1C) = *(float *)(ent + 0x1C) + 0.05f;
                    if (*(float *)(ent + 0x1C) > 1.5f) {
                        *(int *)(ent + 0x1C) = 0xBF800000;
                    }
                }
                i += 1;
            } while (i < 0x150);
        }
        return;
    case 2:
    case 3:
        func_001AFC10(arg0, st);
        break;
    }
}
